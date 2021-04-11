package io.literal.service;

import android.app.Service;
import android.content.Intent;
import android.net.Uri;
import android.os.IBinder;
import android.util.Log;
import android.util.Pair;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.services.s3.AmazonS3URI;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Optional;
import java.util.function.BinaryOperator;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.Callback3;
import io.literal.lib.DomainMetadata;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.ManyCallback;
import io.literal.model.Annotation;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.Target;
import io.literal.model.TimeState;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ArchiveRepository;
import io.literal.repository.AuthenticationRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.NotificationRepository;
import io.literal.ui.MainApplication;
import kotlin.jvm.functions.Function0;

public class AnnotationService extends Service {

    public static String ACTION_CREATE_ANNOTATIONS = "ACTION_CREATE_ANNOTATIONS";
    public static String ACTION_BROADCAST_CREATED_ANNOTATIONS = "ACTION_BROADCAST_CREATED_ANNOTATIONS";

    public static String EXTRA_ANNOTATIONS = "EXTRA_ANNOTATIONS";
    public static String EXTRA_DOMAIN_METADATA = "EXTRA_DOMAIN_METADATA";

    public AnnotationService() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        AWSMobileClientFactory.initializeClient(getApplicationContext());
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
            ((MainApplication) getApplication()).getThreadPoolExecutor().execute(() -> {
                try {
                    AWSMobileClientFactory.getInitializationLatch().await();
                    handleCreateAnnotation(
                            JsonArrayUtil.parseJsonObjectArray(new JSONArray(extraAnnotation), new Annotation[0], Annotation::fromJson),
                            extraDomainMetadata != null ? DomainMetadata.fromJson(new JSONObject(extraDomainMetadata)) : null,
                            onFinish
                    );
                } catch (JSONException ex) {
                    onFinish.invoke(ex, null);
                } catch (InterruptedException ex) {
                    onFinish.invoke(ex, null);
                }
            });

        } else {
            onFinish.invoke(null, null);
        }
        return START_REDELIVER_INTENT;
    }

    private void handleCreateAnnotation(Annotation[] annotations, DomainMetadata domainMetadata, Callback<Exception, Void> onFinish) {
        Callback<Exception, Void> onFinishWithNotification = (e, _v) -> {
            if (e != null) {
                try {
                    Intent intent = new Intent();
                    intent.setAction(ACTION_BROADCAST_CREATED_ANNOTATIONS);
                    intent.putExtra(
                            EXTRA_ANNOTATIONS,
                            JsonArrayUtil.stringifyObjectArray(annotations, Annotation::toJson).toString()
                    );
                    LocalBroadcastManager.getInstance(getBaseContext()).sendBroadcast(intent);
                } catch (JSONException ex) {
                    ErrorRepository.captureException(ex);
                }
            }

            if (e == null && domainMetadata != null) {
                NotificationRepository.sourceCreatedNotificationComplete(
                        getBaseContext(),
                        AuthenticationRepository.getUsername(),
                        domainMetadata
                );
            }
            onFinish.invoke(e, _v);
        };

        ArrayList<Pair<Integer, String[]>> archivesToUpload = new ArrayList<>();
        for (Annotation annotation : annotations) {
            for (Target item : annotation.getTarget()) {
                if (!item.getType().equals(Target.Type.SPECIFIC_TARGET)) {
                    continue;
                }
                for (State value : Optional.ofNullable(((SpecificTarget) item).getState()).orElse(new State[0])) {
                    if (!value.getType().equals(State.Type.TIME_STATE)) {
                        continue;
                    }
                    String[] cached = Optional.ofNullable(((TimeState) value).getCached()).orElse(new String[0]);
                    for (int i = 0; i < cached.length; i++) {
                        Uri uri = Uri.parse(cached[i]);
                        if (!uri.getScheme().equals("file")) {
                            continue;
                        }
                        archivesToUpload.add(new Pair<>(i, cached));
                    }
                }
            }
        }

        if (archivesToUpload.size() == 0) {
            AnnotationRepository.createAnnotations(getBaseContext(), annotations, (ex, result) -> onFinishWithNotification.invoke(ex, null));
            return;
        }

        ManyCallback<Exception, AmazonS3URI> manyCallback = new ManyCallback<>(archivesToUpload.size(), (e, amazonS3URIs) -> {
            if (e != null) {
                onFinishWithNotification.invoke(e, null);
                return;
            }

            for (int i = 0; i < amazonS3URIs.size(); i++) {
                Pair<Integer, String[]> uploadedArchive = archivesToUpload.get(i);
                String localFilePath = uploadedArchive.second[uploadedArchive.first];
                uploadedArchive.second[uploadedArchive.first] = amazonS3URIs.get(i).toString();

                try {
                    boolean deleted = new File(new URI(localFilePath)).delete();
                    if (!deleted) {
                        ErrorRepository.captureException(new Exception("Unable to delete file: " + localFilePath));
                    }
                } catch (URISyntaxException ex) {
                    ErrorRepository.captureException(ex);
                }
            }

            AnnotationRepository.createAnnotations(getBaseContext(), annotations, (ex, result) -> {
                if (ex != null) {
                    onFinishWithNotification.invoke(ex, null);
                    return;
                }
                onFinishWithNotification.invoke(null, null);
            });
        });

        HashMap<Integer, Pair<Long, Long>> uploadProgressById = new HashMap<>();
        Function0<Void> onDisplayNotificationProgress = () -> {
            if (domainMetadata == null) {
                return null;
            }

            Pair<Long, Long> progress = uploadProgressById.values().stream()
                    .reduce(new Pair<>(0L, 0L), (acc, item) -> new Pair<>(acc.first + item.first, acc.second + item.second));
            int progressOutOf100 = progress.second.compareTo(0L) == 0 || progress.first.compareTo(0L) == 0
                    ? 0
                    : (int) (((double) progress.first / progress.second) * 100);

            NotificationRepository.sourceCreatedNotificationStart(
                    getBaseContext(),
                    AuthenticationRepository.getUsername(),
                    domainMetadata,
                    new Pair<>(100, Math.max(progressOutOf100 - 5, 0)) // subtract 5 to fake mutation progress
            );

            return null;
        };

        Callback3<Integer, Long, Long> onUploadProgress = (e, id, bytesCurrent, bytesTotal) -> {
            uploadProgressById.put(id, new Pair<>(bytesCurrent, bytesTotal));
            onDisplayNotificationProgress.invoke();
        };

        for (int i = 0; i < archivesToUpload.size(); i++) {
            Pair<Integer, String[]> archive = archivesToUpload.get(i);
            String filePath = archive.second[archive.first];

            try {
                TransferObserver transferObserver = ArchiveRepository.upload(
                        getBaseContext(),
                        new File(new URI(filePath)),
                        AuthenticationRepository.getIdentityId(),
                        manyCallback.getCallback(i),
                        onUploadProgress
                );
                uploadProgressById.put(transferObserver.getId(), new Pair<>(transferObserver.getBytesTransferred(), transferObserver.getBytesTotal()));
            } catch (URISyntaxException ex) {
                manyCallback.getCallback(i).invoke(ex, null);
            }
        }
        onDisplayNotificationProgress.invoke();
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        throw new UnsupportedOperationException("Not yet implemented");
    }
}