package io.literal.service;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.IBinder;
import android.util.Log;
import android.util.Pair;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.services.s3.AmazonS3URI;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URI;
import java.security.NoSuchAlgorithmException;
import java.security.cert.PKIXRevocationChecker;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.stream.Collectors;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.AmazonS3URILib;
import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.AnnotationLib;
import io.literal.lib.Callback;
import io.literal.lib.Constants;
import io.literal.lib.Crypto;
import io.literal.lib.DateUtil;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.ExternalTarget;
import io.literal.model.ResourceType;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.StorageObject;
import io.literal.model.Target;
import io.literal.model.TextualTarget;
import io.literal.model.TimeState;
import io.literal.model.User;
import io.literal.model.WebArchive;
import io.literal.repository.AnalyticsRepository;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.NotificationRepository;
import io.literal.ui.MainApplication;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.functions.Function3;

public class AnnotationService extends Service {
    public static String ACTION_BROADCAST_CREATED_ANNOTATIONS = Constants.NAMESPACE + "ACTION_BROADCAST_CREATED_ANNOTATIONS";
    public static String ACTION_BROADCAST_UPDATED_ANNOTATION = Constants.NAMESPACE + "ACTION_BROADCAST_UPDATED_ANNOTATION";

    public static String EXTRA_ID = Constants.NAMESPACE + "EXTRA_ID";
    public static String EXTRA_ANNOTATIONS = Constants.NAMESPACE + "EXTRA_ANNOTATIONS";
    public static String EXTRA_ANNOTATION = Constants.NAMESPACE + "EXTRA_ANNOTATION";

    private User user;
    private UserStateListener userStateListener;

    public AnnotationService() {
    }

    public static void broadcastCreatedAnnotations(Context context, String intentId, Annotation[] annotations) {
        try {
            Intent intent = new Intent();
            intent.setAction(ACTION_BROADCAST_CREATED_ANNOTATIONS);
            intent.putExtra(
                    EXTRA_ANNOTATIONS,
                    JsonArrayUtil.stringifyObjectArray(annotations, Annotation::toJson).toString()
            );
            intent.putExtra(EXTRA_ID, intentId);
            LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
        } catch (JSONException ex) {
            ErrorRepository.captureException(ex);
        }
    }

    public static void broadcastUpdatedAnnotation(Context context, String intentId, Annotation annotation) {
        try {
            Intent intent = new Intent();
            intent.setAction(ACTION_BROADCAST_UPDATED_ANNOTATION);
            intent.putExtra(
                    EXTRA_ANNOTATION,
                    annotation.toJson().toString()
            );
            intent.putExtra(EXTRA_ID, intentId);
            LocalBroadcastManager.getInstance(context).sendBroadcast(intent);
        } catch (JSONException ex) {
            ErrorRepository.captureException(ex);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        initialize(getBaseContext());
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        if (userStateListener != null) {
            AWSMobileClient.getInstance().removeUserStateListener(userStateListener);
            userStateListener = null;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        CompletableFuture<Void> future = new CompletableFuture<>();
        Optional<CreateAnnotationIntent> createAnnotationIntentOpt = CreateAnnotationIntent.fromIntent(getBaseContext(), intent);
        if (createAnnotationIntentOpt.isPresent()) {
            CreateAnnotationIntent createAnnotationIntent = createAnnotationIntentOpt.get();

            try {
                JSONObject data = new JSONObject();
                data.put("handler", AnnotationService.class.getName());
                data.put("data", createAnnotationIntent.toJSON());
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_HANDLE_INTENT_START, data);
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }

            ((MainApplication) getApplication()).getThreadPoolExecutor().execute(() -> initialize(getBaseContext())
                    .thenCompose((user) ->
                            handleCreateAnnotation(
                                    getBaseContext(),
                                    user,
                                    createAnnotationIntent
                            )
                    ).whenComplete((_void, ex) -> {
                        if (ex != null) {
                            future.completeExceptionally(ex);
                        } else {
                            future.complete(null);
                        }
                    }));
        } else {
            future.complete(null);
        }

        future.whenComplete((_void, e) -> {
            if (e != null) {
                ErrorRepository.captureException(e);
            }

            try {
                JSONObject data = new JSONObject();
                data.put("result", e != null ? "error" : "success");
                if (createAnnotationIntentOpt.isPresent()) {
                    CreateAnnotationIntent createAnnotationIntent = createAnnotationIntentOpt.get();
                    data.put("data", createAnnotationIntent.toJSON());
                }
                AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_HANDLE_INTENT_COMPLETE, data);
            } catch (JSONException ex) {
                ErrorRepository.captureException(ex);
            }
            stopSelfResult(startId);
        });

        return START_REDELIVER_INTENT;
    }

    private CompletableFuture<User> initialize(Context context) {
        if (user != null) {
            return CompletableFuture.completedFuture(user);
        }
        AnalyticsRepository.initialize(getApplication());
        CompletableFuture<User> userFuture = AWSMobileClientFactory.initializeClient(getApplicationContext()).thenCompose(User::getInstance);

        userFuture.whenComplete((instance, error) -> {
            userStateListener = User.subscribe((e1, newUser) -> {
                user = newUser;
                return null;
            });

            if (error != null) {
                ErrorRepository.captureException(error);
                return;
            }

            user = instance;
        });

        return userFuture;
    }

    private CompletableFuture<HashMap<String, AmazonS3URI>> uploadWebArchives(
            Context context,
            User user,
            CreateAnnotationIntent intent,
            Function1<Integer, Void> onUploadProgress
    ) {
        HashMap<Integer, Pair<Long, Long>> uploadProgressById = new HashMap<>();
        Function3<Integer, Long, Long, Void> handleUploadProgress = (id, bytesCurrent, bytesTotal) -> {
            uploadProgressById.put(id, new Pair<>(bytesCurrent, bytesTotal));
            Pair<Long, Long> progress = uploadProgressById.values().stream()
                    .reduce(new Pair<>(0L, 0L), (acc, item) -> new Pair<>(acc.first + item.first, acc.second + item.second));
            int progressOutOf100 = progress.second.compareTo(0L) == 0 || progress.first.compareTo(0L) == 0
                    ? 0
                    : (int) (((double) progress.first / progress.second) * 100);
            onUploadProgress.invoke(progressOutOf100);
            return null;
        };

        List<CompletableFuture<Pair<String, AmazonS3URI>>> storageObjectUploadFutures = intent.getWebArchives().orElse(new HashMap<>()).entrySet().stream().map(entry -> {
            String annotationId = entry.getKey();
            WebArchive webArchive = entry.getValue();

            CompletableFuture<AmazonS3URI> future;
            if (!webArchive.getStorageObject().getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                future = CompletableFuture.completedFuture(webArchive.getStorageObject().getAmazonS3URI(context, user));
            } else {
                future = webArchive.compile(context, user)
                        .thenCompose(compiledWebArchive -> compiledWebArchive.getStorageObject().upload(context, user, handleUploadProgress));
            }

            return future.thenApply((storageObjectURI) -> new Pair<>(annotationId, storageObjectURI));

        }).collect(Collectors.toList());

        if (storageObjectUploadFutures.size() > 0) {
            onUploadProgress.invoke(0);
        }

        return CompletableFuture.allOf(storageObjectUploadFutures.toArray(new CompletableFuture[0]))
                    .thenApply((_void) -> {
                        if (storageObjectUploadFutures.size() > 0) {
                            onUploadProgress.invoke(100);
                        }

                        return storageObjectUploadFutures.stream()
                                .map((f) -> f.getNow(null))
                                .collect(
                                        HashMap::new,
                                        (agg, pair) -> agg.put(pair.first, pair.second),
                                        HashMap::putAll
                                );
                    });
    }

    private CompletableFuture<HashMap<String, AmazonS3URI>> uploadScreenshots(
            Context context,
            User user,
            CreateAnnotationIntent intent,
            Function1<Integer, Void> onUploadProgress
    ) {
        HashMap<Integer, Pair<Long, Long>> uploadProgressById = new HashMap<>();
        Function3<Integer, Long, Long, Void> handleUploadProgress = (id, bytesCurrent, bytesTotal) -> {
            uploadProgressById.put(id, new Pair<>(bytesCurrent, bytesTotal));
            Pair<Long, Long> progress = uploadProgressById.values().stream()
                    .reduce(new Pair<>(0L, 0L), (acc, item) -> new Pair<>(acc.first + item.first, acc.second + item.second));
            int progressOutOf100 = progress.second.compareTo(0L) == 0 || progress.first.compareTo(0L) == 0
                    ? 0
                    : (int) (((double) progress.first / progress.second) * 100);
            onUploadProgress.invoke(progressOutOf100);
            return null;
        };

        List<CompletableFuture<Pair<String, AmazonS3URI>>> storageObjectUploadFutures = Arrays.stream(intent.getAnnotations())
                .map(annotation -> Arrays.stream(annotation.getTarget())
                    .filter(target -> {
                        if (!(target instanceof ExternalTarget)) {
                            return false;
                        }
                        ExternalTarget externalTarget = (ExternalTarget) target;

                        return externalTarget.getResourceType().equals(ResourceType.IMAGE) && externalTarget.getId().getType().equals(ExternalTarget.Id.Type.STORAGE_OBJECT);
                    })
                    .findFirst()
                    .map((target) -> {
                        ExternalTarget externalTarget = (ExternalTarget) target;

                        StorageObject storageObject = externalTarget.getId().getStorageObject();
                        CompletableFuture<AmazonS3URI> uploadFuture;
                        if (!storageObject.getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                            uploadFuture = CompletableFuture.completedFuture(storageObject.getAmazonS3URI(context, user));
                        } else {
                            uploadFuture = storageObject.upload(context, user, handleUploadProgress);
                        }

                        return uploadFuture.thenApply((uri) -> new Pair<>(annotation.getId(), uri));
                    }))
                .filter(Optional::isPresent)
                .map(o -> o.orElse(null))
                .collect(Collectors.toList());

        if (storageObjectUploadFutures.size() > 0) {
            onUploadProgress.invoke(0);
        }

        return CompletableFuture.allOf(storageObjectUploadFutures.toArray(new CompletableFuture[0]))
                .thenApply((_void) -> {
                    if (storageObjectUploadFutures.size() > 0) {
                        onUploadProgress.invoke(100);
                    }

                    return storageObjectUploadFutures.stream().map((f) -> f.getNow(null)).collect(
                            HashMap::new,
                            (agg, pair) -> agg.put(pair.first, pair.second),
                            HashMap::putAll
                    );
                });
    }

    private CompletableFuture<Void> handleCreateAnnotation(Context context, User user, CreateAnnotationIntent createAnnotationIntent) {
        Function1<Integer, Void> onDisplayNotificationProgress = (Integer uploadProgress) -> {
            if (!createAnnotationIntent.getFaviconBitmap().isPresent() || !createAnnotationIntent.getDisplayURI().isPresent()) {
                return null;
            }

            if (!createAnnotationIntent.getDisableNotification()) {
                NotificationRepository.annotationsCreatedNotificationStart(
                        context,
                        createAnnotationIntent.getId().hashCode(),
                        createAnnotationIntent.getAnnotations().length,
                        createAnnotationIntent.getFaviconBitmap(),
                        new Pair<>(100, Math.max(uploadProgress - 5, 0)) // subtract 5 to fake mutation progress
                );
            }

            return null;
        };

        CompletableFuture<HashMap<String, AmazonS3URI>> uploadedWebArchivesFuture = uploadWebArchives(context, user, createAnnotationIntent, onDisplayNotificationProgress);
        CompletableFuture<HashMap<String, AmazonS3URI>> uploadedScreenshotFuture = uploadScreenshots(context, user, createAnnotationIntent, onDisplayNotificationProgress);
        return CompletableFuture.allOf(uploadedWebArchivesFuture, uploadedScreenshotFuture)
                .thenApply(_void -> {
                   HashMap<String, AmazonS3URI> uploadedWebArchives = uploadedWebArchivesFuture.getNow(new HashMap<>());
                   HashMap<String, AmazonS3URI> uploadedScreenshots = uploadedScreenshotFuture.getNow(new HashMap<>());
                   if (uploadedWebArchives.size() == 0 && uploadedScreenshots.size() == 0) {
                       onDisplayNotificationProgress.invoke(100);
                   }

                   List<Annotation> updatedAnnotations = Arrays.stream(createAnnotationIntent.getAnnotations())
                            .map(annotation -> {
                                if (uploadedWebArchives.containsKey(annotation.getId())) {
                                    AmazonS3URI webArchiveURI = uploadedWebArchives.get(annotation.getId());
                                    return Arrays.stream(annotation.getTarget())
                                            .filter(t -> t instanceof SpecificTarget)
                                            .findFirst()
                                            .map((specificTarget) -> {
                                                SpecificTarget updatedSpecifcTarget = new SpecificTarget.Builder((SpecificTarget) specificTarget)
                                                        .setState(new State[]{
                                                                new TimeState(
                                                                        new URI[]{AmazonS3URILib.toHTTPs(context, webArchiveURI)},
                                                                        new String[]{DateUtil.toISO8601UTC(new Date())}
                                                                )
                                                        })
                                                        .build();

                                                return annotation.updateTarget(updatedSpecifcTarget);
                                            })
                                            .orElse(annotation);
                                }

                                if (uploadedScreenshots.containsKey(annotation.getId())) {
                                    AmazonS3URI screenshotURI = uploadedScreenshots.get(annotation.getId());
                                    return Arrays.stream(annotation.getTarget())
                                            .filter(t -> t instanceof ExternalTarget)
                                            .findFirst()
                                            .map((target) -> {
                                                ExternalTarget externalTarget = (ExternalTarget) target;
                                                ExternalTarget updatedExternalTarget = new ExternalTarget.Builder(externalTarget)
                                                        .setId(ExternalTarget.Id.fromString(AmazonS3URILib.toHTTPs(context, screenshotURI).toString()))
                                                        .build();

                                                return annotation.updateTarget(updatedExternalTarget, externalTarget.getId().toString());
                                            })
                                            .orElse(annotation);
                                }

                                return annotation;
                            }).collect(Collectors.toList());

                    return AnnotationRepository.createAnnotations(
                            AppSyncClientFactory.getInstanceForUser(context, user),
                            updatedAnnotations.toArray(new Annotation[0])
                    );
                })
                .thenApply(_results -> (Void) null)
                .whenComplete((_void, e) -> {
                    if (e != null) {
                        ErrorRepository.captureException(e);
                        if (!createAnnotationIntent.getDisableNotification()) {
                            NotificationRepository.annotationCreatedNotificationError(
                                    context,
                                    user.getAppSyncIdentity(),
                                    createAnnotationIntent.getId().hashCode()
                            );
                        }
                    } else {
                        broadcastCreatedAnnotations(context, createAnnotationIntent.getId(), createAnnotationIntent.getAnnotations());

                        if (createAnnotationIntent.getDisableNotification()) {
                            return;
                        }

                        if (createAnnotationIntent.getAnnotations().length == 1) {
                            Annotation annotation = createAnnotationIntent.getAnnotations()[0];
                            Uri annotationUri = Uri.parse(
                                    WebRoutes.creatorsIdAnnotationCollectionIdAnnotationId(
                                            user.getAppSyncIdentity(),
                                            Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT,
                                            AnnotationLib.idComponentFromId(annotation.getId())
                                    )
                            );
                            Arrays.stream(annotation.getTarget())
                                    .filter(t -> t instanceof TextualTarget)
                                    .findFirst()
                                    .flatMap(target -> {
                                        String value = ((TextualTarget) target).getValue();
                                        return value != null && value.length() > 0 ? Optional.of(value) : Optional.empty();
                                    })
                                    .ifPresent((annotationTargetText) -> NotificationRepository.annotationCreatedNotification(
                                            context,
                                            createAnnotationIntent.getId().hashCode(),
                                            annotationUri,
                                            annotationTargetText,
                                            createAnnotationIntent.getFaviconBitmap()
                                    ));
                        } else {
                            Function1<Target, Optional<ExternalTarget>> pred = (t) -> {
                                if (t instanceof SpecificTarget) {
                                    SpecificTarget specificTarget = (SpecificTarget) t;
                                    if (specificTarget.getSource() instanceof ExternalTarget) {
                                        return Optional.of((ExternalTarget) specificTarget.getSource());
                                    }
                                }
                                return Optional.empty();
                            };
                            Pair<String, Optional<String>> targetIdComponentHostPair = Arrays.stream(createAnnotationIntent.getAnnotations())
                                    .flatMap((a) -> Arrays.stream(a.getTarget()))
                                    .filter((t) -> pred.invoke(t).isPresent())
                                    .findFirst()
                                    .flatMap(pred::invoke)
                                    .flatMap((t) -> Optional.ofNullable(t.getId().getIri()))
                                    .flatMap((iri) -> {
                                            try {
                                                return Optional.of(Pair.create(
                                                        Crypto.sha256Hex(iri),
                                                        Optional.of(Uri.parse(iri).getHost())
                                                ));
                                            } catch (NoSuchAlgorithmException nse) {
                                                ErrorRepository.captureException(nse);
                                                return Optional.empty();
                                            }
                                    })
                                    .orElse(Pair.create(
                                            Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT,
                                            Optional.empty()
                                    ));
                            Uri notificationUri = Uri.parse(
                                    WebRoutes.creatorsIdAnnotationCollectionId(
                                            user.getAppSyncIdentity(),
                                            targetIdComponentHostPair.first
                                    )
                            );

                            NotificationRepository.multipleAnnotationsCreatedNotification(
                                    context,
                                    createAnnotationIntent.getId().hashCode(),
                                    notificationUri,
                                    createAnnotationIntent.getAnnotations().length,
                                    targetIdComponentHostPair.second,
                                    createAnnotationIntent.getFaviconBitmap()
                            );
                        }
                    }
                });
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
}