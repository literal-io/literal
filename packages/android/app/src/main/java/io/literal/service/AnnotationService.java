package io.literal.service;

import android.app.Activity;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.util.Pair;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobile.client.UserStateListener;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.CompletableFuture;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.Callback3;
import io.literal.lib.DomainMetadata;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.ManyCallback;
import io.literal.model.Annotation;
import io.literal.model.ExternalTarget;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.StorageObject;
import io.literal.model.Target;
import io.literal.model.TimeState;
import io.literal.model.User;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.NotificationRepository;
import io.literal.ui.MainApplication;
import kotlin.jvm.functions.Function1;
import kotlin.jvm.functions.Function2;

public class AnnotationService extends Service {

    public static String ACTION_CREATE_ANNOTATIONS = "ACTION_CREATE_ANNOTATIONS";
    public static String ACTION_BROADCAST_CREATED_ANNOTATIONS = "ACTION_BROADCAST_CREATED_ANNOTATIONS";
    public static String ACTION_BROADCAST_UPDATED_ANNOTATION = "ACTION_BROADCAST_UPDATED_ANNOTATION";

    public static String EXTRA_ID = "EXTRA_ID";
    public static String EXTRA_ANNOTATIONS = "EXTRA_ANNOTATIONS";
    public static String EXTRA_ANNOTATION = "EXTRA_ANNOTATION";
    public static String EXTRA_DOMAIN_METADATA = "EXTRA_DOMAIN_METADATA";

    private User user;
    private UserStateListener userStateListener;

    public AnnotationService() {
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

    public static Intent getCreateAnnotationsIntent(Activity activity, @NotNull Annotation[] annotations, DomainMetadata domainMetadata) {
        try {
            Intent serviceIntent = new Intent(activity, AnnotationService.class);
            serviceIntent.setAction(AnnotationService.ACTION_CREATE_ANNOTATIONS);
            serviceIntent.putExtra(
                    AnnotationService.EXTRA_ID,
                    UUID.randomUUID().toString()
            );
            serviceIntent.putExtra(
                    AnnotationService.EXTRA_ANNOTATIONS,
                    JsonArrayUtil.stringifyObjectArray(annotations, Annotation::toJson).toString()
            );
            if (domainMetadata != null) {
                serviceIntent.putExtra(
                        AnnotationService.EXTRA_DOMAIN_METADATA,
                        domainMetadata.toJson(activity).toString()
                );
            }
            return serviceIntent;
        } catch (JSONException ex) {
            ErrorRepository.captureException(ex);
            return null;
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Callback<Exception, Void> onFinish = (e, _d) -> {
            if (e != null) {
                ErrorRepository.captureException(e);
            }
            stopSelfResult(startId);
        };

        if (intent.getAction().equals(ACTION_CREATE_ANNOTATIONS)) {
            String extraAnnotation = intent.getStringExtra(EXTRA_ANNOTATIONS);
            String extraDomainMetadata = intent.getStringExtra(EXTRA_DOMAIN_METADATA);
            String extraId = intent.getStringExtra(EXTRA_ID);
            ((MainApplication) getApplication()).getThreadPoolExecutor().execute(() -> initialize(getBaseContext()).whenComplete((user, e) -> {
                if (e != null) {
                    onFinish.invoke((Exception) e, null);
                }

                try {
                    handleCreateAnnotation(
                            getBaseContext(),
                            extraId,
                            user,
                            JsonArrayUtil.parseJsonObjectArray(new JSONArray(extraAnnotation), new Annotation[0], Annotation::fromJson),
                            extraDomainMetadata != null ? DomainMetadata.fromJson(new JSONObject(extraDomainMetadata)) : null,
                            onFinish
                    );
                } catch (JSONException ex) {
                    onFinish.invoke(ex, null);
                }
            }));

        } else {
            onFinish.invoke(null, null);
        }

        return START_REDELIVER_INTENT;
    }

    private CompletableFuture<User> initialize(Context context) {
        if (user != null) {
            return CompletableFuture.completedFuture(user);
        }

        CompletableFuture<User> userFuture = new CompletableFuture<>();
        AWSMobileClientFactory.initializeClient(context, new com.amazonaws.mobile.client.Callback<UserStateDetails>() {
            @Override
            public void onResult(UserStateDetails result) {
                User.getInstance(result).whenComplete((user, error) -> {
                   if (error != null) {
                       userFuture.completeExceptionally(error);
                    } else {
                       userFuture.complete(user);
                   }
                });

                userStateListener = User.subscribe((e, instance) -> {
                    if (e != null) {
                        ErrorRepository.captureException(e);
                    }
                    user = instance;
                    return null;
                });
            }

            @Override
            public void onError(Exception e) {
                ErrorRepository.captureException(e);
                userFuture.completeExceptionally(e);
                userStateListener = User.subscribe((e1, instance) -> {
                    if (e1 != null) {
                        ErrorRepository.captureException(e1);
                    }
                    user = instance;
                    return null;
                });
            }
        });

        return userFuture;
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

    private void uploadStorageRepositoryFiles(
        Context context,
        User user,
        Annotation[] annotations,
        Function1<Integer, Void> onUploadProgress,
        Callback<Exception, Annotation[]> onComplete
    ) {

        ArrayList<Function1<Callback<Exception, StorageObject>, TransferObserver>> uploadThunks = new ArrayList<>();
        HashMap<Integer, Pair<Long, Long>> uploadProgressById = new HashMap<>();
        Callback3<Integer, Long, Long> handleUploadProgress = (e, id, bytesCurrent, bytesTotal) -> {
            uploadProgressById.put(id, new Pair<>(bytesCurrent, bytesTotal));
            Pair<Long, Long> progress = uploadProgressById.values().stream()
                    .reduce(new Pair<>(0L, 0L), (acc, item) -> new Pair<>(acc.first + item.first, acc.second + item.second));
            int progressOutOf100 = progress.second.compareTo(0L) == 0 || progress.first.compareTo(0L) == 0
                    ? 0
                    : (int) (((double) progress.first / progress.second) * 100);
            onUploadProgress.invoke(progressOutOf100);
        };

        for (Annotation annotation : annotations) {
            for (Target item : annotation.getTarget()) {
                if (item.getType().equals(Target.Type.EXTERNAL_TARGET)) {
                    ExternalTarget.Id id = ((ExternalTarget) item).getId();
                    if (id.getType().equals(ExternalTarget.Id.Type.STORAGE_OBJECT)) {
                        StorageObject storageObject = id.getStorageObject();
                        if (storageObject.getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                            uploadThunks.add(onUploadComplete -> storageObject.upload(
                                    context,
                                    user,
                                    (e, uri1) -> {
                                        if (e != null) {
                                            onUploadComplete.invoke(e, null);
                                        } else {
                                            onUploadComplete.invoke(
                                                    null,
                                                    storageObject
                                            );
                                        }
                                    },
                                    handleUploadProgress
                            ));
                        }
                    }
                }

                if (item.getType().equals(Target.Type.SPECIFIC_TARGET)) {
                    for (State value : Optional.ofNullable(((SpecificTarget) item).getState()).orElse(new State[0])) {
                        if (!value.getType().equals(State.Type.TIME_STATE)) {
                            continue;
                        }
                        StorageObject[] cached = Optional.ofNullable(((TimeState) value).getCached()).orElse(new StorageObject[0]);
                        for (int i = 0; i < cached.length; i++) {
                            StorageObject storageObject = cached[i];
                            if (!storageObject.getStatus().equals(StorageObject.Status.UPLOAD_REQUIRED)) {
                                continue;
                            }
                            uploadThunks.add(onUploadComplete -> storageObject.upload(
                                    context,
                                    user,
                                    (e, uri1) -> {
                                        if (e != null) {
                                            onUploadComplete.invoke(e, null);
                                        } else {
                                            onUploadComplete.invoke(
                                                    null,
                                                    storageObject
                                            );
                                        }
                                    },
                                    handleUploadProgress
                            ));
                        }
                    }
                }
            }
        }

        if (uploadThunks.size() == 0) {
            onComplete.invoke(null, annotations);
            return;
        }

        ManyCallback<Exception, StorageObject> manyCallback = new ManyCallback<>(uploadThunks.size(), (e, storageObjects) -> {
            if (e != null) {
                onComplete.invoke(e, null);
                return;
            }

            for (int i = 0; i < storageObjects.size(); i++) {
                StorageObject storageObject = storageObjects.get(i);
                boolean deleted = storageObject.deleteFile(context);
                if (!deleted) {
                    ErrorRepository.captureException(new Exception("Unable to delete file: " + storageObject.getFile(context).toURI().toString()));
                }
            }

            onComplete.invoke(e, annotations);
        });

        for (int i = 0; i < uploadThunks.size(); i++) {
            uploadThunks.get(i).invoke(manyCallback.getCallback(i));
        }
        onUploadProgress.invoke(0);
    }

    private void handleCreateAnnotation(Context context, String intentId, User user, Annotation[] annotations, DomainMetadata domainMetadata, Callback<Exception, Void> onFinish) {
        Callback<Exception, Void> onFinishWithNotification = (e, _v) -> {
            if (e != null) {
                NotificationRepository.sourceCreatedNotificationError(
                        context,
                        user.getUsername(),
                        domainMetadata
                );
               onFinish.invoke(e, _v);
               return;
            }

            broadcastCreatedAnnotations(context, intentId, annotations);

            if (domainMetadata != null) {
                NotificationRepository.sourceCreatedNotificationComplete(
                        context,
                        user.getUsername(),
                        domainMetadata
                );
            }

            onFinish.invoke(e, _v);
        };

        Function1<Integer, Void> onDisplayNotificationProgress = (Integer uploadProgress) -> {
            if (domainMetadata == null) {
                return null;
            }

            NotificationRepository.sourceCreatedNotificationStart(
                    context,
                    user.getUsername(),
                    domainMetadata,
                    new Pair<>(100, Math.max(uploadProgress - 5, 0)) // subtract 5 to fake mutation progress
            );

            return null;
        };

        uploadStorageRepositoryFiles(
                context,
                user,
                annotations,
                onDisplayNotificationProgress,
                (e, processedAnnotations) ->
                    AnnotationRepository.createAnnotations(
                        getBaseContext(),
                        processedAnnotations,
                            (e1, data) -> onFinishWithNotification.invoke(e1, null)
                    )
        );
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
}