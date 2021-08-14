package io.literal.model;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Pair;
import java.util.Collection;
import java.util.Optional;

import io.literal.repository.NotificationRepository;

public class CreateAnnotationNotification {
    private final int id;
    private boolean didError;
    private Long bytesTotal;
    private Long bytesCurrent;

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

    private int count;

    public CreateAnnotationNotification(int id, boolean didError, Long bytesTotal, Long bytesCurrent, int count) {
        this.id = id;
        this.didError = didError;
        this.bytesTotal = bytesTotal;
        this.bytesCurrent = bytesCurrent;
        this.count = count;
    }
    public CreateAnnotationNotification(int id, boolean didError, Long bytesTotal, Long bytesCurrent) {
        this(id, didError, bytesTotal, bytesCurrent, 1);
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
                100L,
                progressOutOf100,
                notifications.size()
            )
        );
    }

    public void dispatch(Context context, Optional<Bitmap> faviconBitmap) {
        if (didError) {
            // error notification
        } else if (bytesCurrent.equals(bytesTotal)) {
            // complete notification
        } else {
            // progress notification
            int progressCurrent;
            int progressTotal = 100;
            if (!bytesTotal.equals(100L)) {
                progressCurrent = bytesCurrent.equals(0L) ? 0 : (int) (((double) bytesCurrent / bytesTotal) * 100);
            } else {
                progressCurrent = bytesCurrent.intValue();
            }

            NotificationRepository.annotationsCreatedNotificationStart(context, id, count, faviconBitmap, new Pair<>(progressCurrent, progressTotal));
        }
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
}