package io.literal.model;

import android.content.Context;
import android.graphics.Bitmap;
import android.net.Uri;
import android.util.Pair;

import java.net.URI;
import java.util.Arrays;
import java.util.Collection;
import java.util.Optional;

import io.literal.lib.Constants;
import io.literal.lib.WebRoutes;
import io.literal.repository.NotificationRepository;
import io.literal.service.CreateAnnotationIntent;

public class CreateAnnotationNotification {
    private final int id;
    private boolean didError;
    private boolean didCompleteMutation;
    private Long bytesTotal;
    private Long bytesCurrent;

    private int count;

    public CreateAnnotationNotification(int id, boolean didError, boolean didCompleteMutation, Long bytesTotal, Long bytesCurrent, int count) {
        this.id = id;
        this.didError = didError;
        this.didCompleteMutation = didCompleteMutation;
        this.bytesTotal = bytesTotal;
        this.bytesCurrent = bytesCurrent;
        this.count = count;
    }

    public CreateAnnotationNotification(int id, boolean didError, boolean didCompleteMutation, Long bytesTotal, Long bytesCurrent) {
        this(id, didError, didCompleteMutation, bytesTotal, bytesCurrent, 1);
    }

    public CreateAnnotationNotification(int id, Long bytesTotal, Long bytesCurrent) {
        this(id, false, false, bytesTotal, bytesCurrent, 1);
    }

    public static Optional<CreateAnnotationNotification> aggregate(Collection<CreateAnnotationNotification> notifications) {
        Optional<CreateAnnotationNotification> erroredNotification = notifications.stream().filter(CreateAnnotationNotification::getDidError).findFirst();
        if (erroredNotification.isPresent()) {
            return erroredNotification;
        }

        Optional<Integer> notificationId = notifications.stream().findFirst().map(CreateAnnotationNotification::getId);
        Pair<Long, Long> progress = notifications.stream()
                .reduce(
                        new Pair<>(0L, 0L),
                        (acc, n) -> new Pair<>(n.getBytesCurrent() + acc.first, n.getBytesTotal() + acc.second),
                        (p1, p2) -> new Pair<>(p1.first + p2.first, p1.second + p2.second)
                );
        Long progressOutOf100 = progress.second.compareTo(0L) == 0 || progress.first.compareTo(0L) == 0
                ? 0L
                : (int) (((double) progress.first / progress.second) * 100);

        return notificationId.map(id -> new CreateAnnotationNotification(
                        id,
                        false,
                        false,
                        100L,
                        progressOutOf100,
                        notifications.size()
                )
        );
    }

    public Status dispatch(Context context, User user, CreateAnnotationIntent intent) {
        if (didError) {
            NotificationRepository.annotationCreatedNotificationError(
                    context,
                    user.getAppSyncIdentity(),
                    id
            );
            return Status.ERROR;
        } else if (bytesCurrent.equals(bytesTotal)) {
            Uri annotationUri = Uri.parse(
                    WebRoutes.creatorsIdAnnotationCollectionId(
                            user.getAppSyncIdentity(),
                            Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT
                    )
            );
            if (count == 1) {
                Optional<String> annotationTargetText = Arrays.stream(intent.getAnnotation().getTarget())
                        .filter(t -> t instanceof TextualTarget)
                        .findFirst()
                        .flatMap(target -> {
                            String value = ((TextualTarget) target).getValue();
                            return value != null && value.length() > 0 ? Optional.of(value) : Optional.empty();
                        });
                NotificationRepository.annotationCreatedNotification(
                        context,
                        id,
                        annotationUri,
                        annotationTargetText,
                        intent.getFaviconBitmap()
                );
            } else {
                NotificationRepository.multipleAnnotationsCreatedNotification(
                        context,
                        id,
                        annotationUri,
                        count,
                        intent.getDisplayURI().map(URI::getHost),
                        intent.getFaviconBitmap()
                );
            }
            return Status.COMPLETE;
        } else {
            // progress notification
            int progressCurrent;
            int progressTotal = 100;
            if (!bytesTotal.equals(100L)) {
                progressCurrent = bytesCurrent.equals(0L) ? 0 : (int) (((double) bytesCurrent / bytesTotal) * 100);
            } else {
                progressCurrent = bytesCurrent.intValue();
            }

            NotificationRepository.annotationsCreatedNotificationStart(context, id, count, intent.getFaviconBitmap(), new Pair<>(progressCurrent, progressTotal));
            return Status.PROGRESS;
        }
    }

    public Long getBytesTotal() {
        return bytesTotal;
    }

    public void setBytesTotal(Long bytesTotal) {
        this.bytesTotal = bytesTotal;
    }

    public Long getBytesCurrent() {
        return bytesCurrent;
    }

    public void setBytesCurrent(Long bytesCurrent) {
        this.bytesCurrent = bytesCurrent;
    }


    public int getId() {
        return id;
    }

    public boolean getDidError() {
        return didError;
    }

    public void setDidError(boolean didError) {
        this.didError = didError;
    }

    public boolean isDidCompleteMutation() {
        return didCompleteMutation;
    }

    public void setDidCompleteMutation(boolean didCompleteMutation) {
        this.didCompleteMutation = didCompleteMutation;
    }

    public enum Status {
        COMPLETE,
        PROGRESS,
        ERROR;
    }
}