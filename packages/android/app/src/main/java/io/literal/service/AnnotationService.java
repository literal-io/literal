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
import com.amazonaws.services.s3.AmazonS3URI;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URI;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.stream.Collectors;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.AmazonS3URILib;
import io.literal.lib.Constants;
import io.literal.lib.DateUtil;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.CreateAnnotationNotification;
import io.literal.model.ExternalTarget;
import io.literal.model.ResourceType;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.StorageObject;
import io.literal.model.Target;
import io.literal.model.TextualTarget;
import io.literal.model.TimeState;
import io.literal.model.User;
import io.literal.repository.AnalyticsRepository;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.NotificationRepository;
import io.literal.ui.MainApplication;
import kotlin.jvm.functions.Function0;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.functions.Function2;
import kotlin.jvm.functions.Function3;

public class AnnotationService extends Service {
    public static String ACTION_BROADCAST_CREATED_ANNOTATIONS = Constants.NAMESPACE + "ACTION_BROADCAST_CREATED_ANNOTATIONS";
    public static String ACTION_BROADCAST_UPDATED_ANNOTATION = Constants.NAMESPACE + "ACTION_BROADCAST_UPDATED_ANNOTATION";

    public static String EXTRA_ID = Constants.NAMESPACE + "EXTRA_ID";
    public static String EXTRA_ANNOTATION = Constants.NAMESPACE + "EXTRA_ANNOTATION";

    private User user;
    private UserStateListener userStateListener;
    private HashMap<String, CreateAnnotationNotification> createAnnotationNotificationsByAnnotationId = new HashMap<>();

    public AnnotationService() {
    }

    public static void broadcastCreatedAnnotation(Context context, String intentId, Annotation annotation) {
        try {
            Intent intent = new Intent();
            intent.setAction(ACTION_BROADCAST_CREATED_ANNOTATIONS);
            intent.putExtra(EXTRA_ANNOTATION, annotation.toJson().toString());
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

    private CompletableFuture<AmazonS3URI> uploadWebArchives(
            Context context,
            User user,
            CreateAnnotationIntent intent,
            Function2<Long, Long, Void> onUploadProgress
    ) {
        return Arrays.stream(intent.getAnnotation().getTarget())
                .<Optional<StorageObject>>map((Target target) -> {
                    if (!(target instanceof SpecificTarget)) {
                        return Optional.empty();
                    }

                    return Arrays.stream(Optional.ofNullable(((SpecificTarget) target).getState()).orElse(new State[0]))
                            .filter((s) -> s.getType().equals(State.Type.TIME_STATE))
                            .findFirst()
                            .flatMap((s) ->
                                    Arrays.stream(((TimeState) s).getCached())
                                            .filter(cached -> cached.getScheme().equals("file"))
                                            .findFirst()
                            )
                            .map((uri) -> StorageObject.create(context, uri));
                })
                .filter(Optional::isPresent)
                .findFirst()
                .map((optionalStorageObject) -> {
                    StorageObject storageObject = optionalStorageObject.get();
                    if (!storageObject.getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                        return CompletableFuture.completedFuture(storageObject.getAmazonS3URI(context, user));
                    } else {
                        return storageObject.upload(context, user, (id, current, total) -> onUploadProgress.invoke(current, total));
                    }
                }).orElse(CompletableFuture.completedFuture(null));
    }

    private void onDisplayAggregateNotification(Context context, CreateAnnotationIntent intent) {
        if (intent.getDisableNotification()) {
            return;
        }

        Optional<CreateAnnotationNotification> aggregateNotification = CreateAnnotationNotification.aggregate(
                createAnnotationNotificationsByAnnotationId.values().stream()
                        .filter((notification) -> notification.getId() == intent.getId().hashCode())
                        .collect(Collectors.toList())
        );

        aggregateNotification.ifPresent((n) -> n.dispatch(context, user, intent));
    }

    private void onUpdateAggregateNotificationProgress(Context context, CreateAnnotationIntent intent, Long bytesCurrent, Long bytesTotal) {
        if (!intent.getDisableNotification()) {
            String annotationId = intent.getAnnotation().getId();
            if (createAnnotationNotificationsByAnnotationId.containsKey(annotationId)) {
                CreateAnnotationNotification notification = createAnnotationNotificationsByAnnotationId.get(annotationId);
                notification.setBytesCurrent(bytesCurrent);
            } else {
                createAnnotationNotificationsByAnnotationId.put(
                        annotationId,
                        new CreateAnnotationNotification(
                                intent.getId().hashCode(),
                                bytesTotal,
                                bytesCurrent
                        )
                );
            }

            this.onDisplayAggregateNotification(context, intent);
        }
    }


    private CompletableFuture<AmazonS3URI> uploadScreenshots(
            Context context,
            User user,
            CreateAnnotationIntent intent,
            Function2<Long, Long, Void> onUploadProgress
    ) {
        return Arrays.stream(intent.getAnnotation().getTarget())
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
                    if (!storageObject.getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                        return CompletableFuture.completedFuture(storageObject.getAmazonS3URI(context, user));
                    } else {
                        return storageObject.upload(
                                context,
                                user,
                                (id, current, total) -> onUploadProgress.invoke(current, total)
                        );
                    }
                }).orElse(CompletableFuture.completedFuture(null));
    }

    private CompletableFuture<Void> handleCreateAnnotation(Context context, User user, CreateAnnotationIntent createAnnotationIntent) {
        CompletableFuture<AmazonS3URI> uploadedWebArchiveFuture = uploadWebArchives(
                context,
                user,
                createAnnotationIntent,
                (currentBytesUploaded, totalBytesUploaded) -> {
                    this.onUpdateAggregateNotificationProgress(context, createAnnotationIntent, currentBytesUploaded, totalBytesUploaded);
                    return null;
                }
        );
        CompletableFuture<AmazonS3URI> uploadedScreenshotFuture = uploadScreenshots(
                context,
                user,
                createAnnotationIntent,
                (currentBytesUploaded, totalBytesUploaded) -> {
                    this.onUpdateAggregateNotificationProgress(context, createAnnotationIntent, currentBytesUploaded, totalBytesUploaded);
                    return null;
                }
        );
        return CompletableFuture.allOf(uploadedWebArchiveFuture, uploadedScreenshotFuture)
                .thenApply(_void -> {
                    AmazonS3URI uploadedWebArchive = uploadedWebArchiveFuture.getNow(null);
                    AmazonS3URI uploadedScreenshot = uploadedScreenshotFuture.getNow(null);

                    Optional<CreateAnnotationNotification> notification = Optional.ofNullable(createAnnotationNotificationsByAnnotationId.get(createAnnotationIntent.getAnnotation().getId()));
                    notification.ifPresent((n) -> {
                        if (!n.getBytesCurrent().equals(n.getBytesTotal())) {
                            n.setBytesCurrent(n.getBytesTotal());
                            this.onDisplayAggregateNotification(context, createAnnotationIntent);
                        }
                    });

                    Annotation annotation = createAnnotationIntent.getAnnotation();
                    if (uploadedWebArchive != null) {
                        return Arrays.stream(annotation.getTarget())
                                .filter(t -> t instanceof SpecificTarget)
                                .findFirst()
                                .map((specificTarget) -> {
                                    SpecificTarget updatedSpecifcTarget = new SpecificTarget.Builder((SpecificTarget) specificTarget)
                                            .setState(new State[]{
                                                    new TimeState(
                                                            new URI[]{AmazonS3URILib.toHTTPs(context, uploadedWebArchive)},
                                                            new String[]{DateUtil.toISO8601UTC(new Date())}
                                                    )
                                            })
                                            .build();

                                    return annotation.updateTarget(updatedSpecifcTarget);
                                })
                                .orElse(annotation);
                    }

                    if (uploadedScreenshot != null) {
                        return Arrays.stream(annotation.getTarget())
                                .filter(t -> t instanceof ExternalTarget)
                                .findFirst()
                                .map((target) -> {
                                    ExternalTarget externalTarget = (ExternalTarget) target;
                                    ExternalTarget updatedExternalTarget = new ExternalTarget.Builder(externalTarget)
                                            .setId(ExternalTarget.Id.fromString(AmazonS3URILib.toHTTPs(context, uploadedScreenshot).toString()))
                                            .build();

                                    return annotation.updateTarget(updatedExternalTarget, externalTarget.getId().toString());
                                })
                                .orElse(annotation);
                    }

                    return annotation;
                })
                .thenCompose((annotation) ->
                    AnnotationRepository.createAnnotationMutation(
                        AppSyncClientFactory.getInstanceForUser(context, user),
                        annotation.toCreateAnnotationInput()
                    ).thenApply((_result) -> {
                        // broadcast the processed annotation, not the annotation on the intent
                        broadcastCreatedAnnotation(context, annotation.getId(), annotation);
                        return (Void) null;
                    })
                )
                .whenComplete((_void, e) -> {
                    Optional<CreateAnnotationNotification> notification = Optional.ofNullable(createAnnotationNotificationsByAnnotationId.get(createAnnotationIntent.getAnnotation().getId()));
                    if (e != null) {
                        ErrorRepository.captureException(e);
                        if (!createAnnotationIntent.getDisableNotification()) {
                            notification.ifPresent(n -> {
                                n.setDidError(true);
                                this.onDisplayAggregateNotification(context, createAnnotationIntent);
                            });
                        }
                        return;
                    }
                    notification.ifPresent(n -> {
                        n.setDidCompleteMutation(true);
                        this.onDisplayAggregateNotification(context, createAnnotationIntent);
                    });
                });
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
}