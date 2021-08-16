package io.literal.repository;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.net.Uri;
import android.util.Pair;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;

import java.net.URI;
import java.util.Optional;

import io.literal.R;
import io.literal.lib.AnnotationLib;
import io.literal.lib.Constants;
import io.literal.lib.WebRoutes;
import io.literal.model.User;
import io.literal.ui.activity.MainActivity;

public class NotificationRepository {

    public static void createNewAnnotationNotificationChannel(Context context) {
        NotificationRepository.createNotificationChannel(
                context,
                context.getString(R.string.annotation_created_notification_channel_id),
                context.getString(R.string.annotation_created_notification_channel_name),
                context.getString(R.string.annotation_created_notification_channel_description),
                NotificationManager.IMPORTANCE_LOW
        );
    }

    private static void createNotificationChannel(
            Context context,
            String notificationChannelId,
            String notificationChannelName,
            String notificationChannelDescription,
            int importance
    ) {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            NotificationChannel chan = new NotificationChannel(
                    notificationChannelId,
                    notificationChannelName,
                    importance
            );
            chan.setDescription(notificationChannelDescription);
            NotificationManager notificationManager = context.getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(chan);
            }
        }
    }

    public static void annotationCreatedNotification(
            Context context,
            int notificationId,
            Uri notificationUri,
            Optional<String> notificationText,
            Optional<Bitmap> favicon
    ) {
        Intent intent = new Intent(context, MainActivity.class);
        intent.setData(notificationUri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.annotation_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.annotation_created_notification_title))
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        favicon.ifPresent(builder::setLargeIcon);
        notificationText.ifPresent((text) -> {
            builder.setContentText(text);
            builder.setStyle(
                    new NotificationCompat.BigTextStyle().bigText(text)
            );
        });
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(notificationId, builder.build());
    }

    public static void multipleAnnotationsCreatedNotification(
            Context context,
            int notificationId,
            Uri notificationUri,
            int annotationCount,
            Optional<String> annotationHost,
            Optional<Bitmap> annotationHostFavicon
    ) {
        Intent intent = new Intent(context, MainActivity.class);
        intent.setData(notificationUri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        String notificationText = annotationHost
                .map(h -> context.getString(R.string.multiple_annotations_created_with_host_notification_description, annotationCount, h))
                .orElse(context.getString(R.string.multiple_annotations_created_notification_description, annotationCount));

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.annotation_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.annotation_created_notification_title))
                .setStyle(
                        new NotificationCompat.BigTextStyle().bigText(notificationText)
                )
                .setContentText(notificationText)
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        annotationHostFavicon.ifPresent(builder::setLargeIcon);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(notificationId, builder.build());
    }

    public static void annotationCreatedNotificationError(Context context, String creatorUsername, int notificationId) {
        Intent intent = new Intent(context, MainActivity.class);
        Uri uri = Uri.parse(WebRoutes.creatorsIdAnnotationCollectionId(
                creatorUsername,
                Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT
        ));
        intent.setData(uri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.annotation_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.annotation_created_error_notification_title))
                .setStyle(
                        new NotificationCompat.BigTextStyle().bigText(context.getString(R.string.annotation_created_error_notification_description))
                )
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(notificationId, builder.build());
    }

    public static void annotationsCreatedNotificationStart(
            Context context,
            int notificationId,
            int annotationCount,
            Optional<Bitmap> annotationHostFavicon,
            Pair<Integer, Integer> progress
    ) {
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.annotation_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(
                        annotationCount > 1
                            ? context.getString(R.string.multiple_annotations_created_start_notification_title)
                                : context.getString(R.string.annotation_created_start_notification_title)
                )
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setProgress(progress.first, progress.second, false)
                .setOngoing(true);
        annotationHostFavicon.ifPresent(builder::setLargeIcon);

        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
        notificationManager.notify(notificationId, builder.build());
    }
}
