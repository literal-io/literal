package io.literal.repository;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.net.Uri;
import android.util.Pair;

import androidx.core.app.NotificationCompat;
import androidx.core.app.NotificationManagerCompat;

import com.amazonaws.amplify.generated.graphql.GetAnnotationQuery;

import org.jetbrains.annotations.NotNull;

import io.literal.R;
import io.literal.lib.AnnotationLib;
import io.literal.lib.Constants;
import io.literal.lib.DomainMetadata;
import io.literal.lib.WebRoutes;
import io.literal.model.User;
import io.literal.ui.activity.MainActivity;

public class NotificationRepository {
    public static void createNewExternalTargetNotificationChannel(Context context) {
        NotificationRepository.createNotificationChannel(
                context,
                context.getString(R.string.source_created_notification_channel_id),
                context.getString(R.string.source_created_notification_channel_name),
                context.getString(R.string.source_created_notification_channel_description),
                NotificationManager.IMPORTANCE_LOW
        );
    }

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

    public static void annotationCreatedNotification(Context context, User user, GetAnnotationQuery.GetAnnotation annotation) {
        Intent intent = new Intent(context, MainActivity.class);
        Uri annotationUri = Uri.parse(
                WebRoutes.creatorsIdAnnotationCollectionIdAnnotationId(
                        user.getAppSyncIdentity(),
                        Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT,
                        AnnotationLib.idComponentFromId(annotation.id())
                )
        );
        intent.setData(annotationUri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        String textualTargetValue = null;
        for (GetAnnotationQuery.Target target : annotation.target()) {
            GetAnnotationQuery.AsTextualTarget textualTarget = target.asTextualTarget();
            if (textualTarget != null) {
                textualTargetValue = textualTarget.value();
                break;
            }
        }

        if (textualTargetValue != null) {
            NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.annotation_created_notification_channel_id))
                    .setSmallIcon(R.drawable.ic_stat_name)
                    .setColor(Color.BLACK)
                    .setContentTitle(context.getString(R.string.annotation_created_notification_title))
                    .setStyle(
                            new NotificationCompat.BigTextStyle().bigText(textualTargetValue)
                    )
                    .setContentText(textualTargetValue)
                    .setPriority(NotificationCompat.PRIORITY_LOW)
                    .setContentIntent(pendingIntent)
                    .setAutoCancel(true);
            NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);
            notificationManager.notify(annotation.id().hashCode(), builder.build());
        }
    }

    public static void sourceCreatedNotificationComplete(Context context, String creatorUsername, @NotNull DomainMetadata targetDomainMetadata) {
        // FIXME: This should link to a source specific view, but none exists currently.

        Intent intent = new Intent(context, MainActivity.class);
        Uri uri = Uri.parse(WebRoutes.creatorsIdAnnotationCollectionId(
                creatorUsername,
                Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT
        ));
        intent.setData(uri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.source_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.source_created_complete_notification_title))
                .setContentText(context.getString(R.string.source_created_complete_notification_description, targetDomainMetadata.getUrl().getHost()))
                .setLargeIcon(targetDomainMetadata.getScaledFaviconWithBackground(context))
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(targetDomainMetadata.getUrl().getHost().hashCode(), builder.build());
    }

    public static void sourceCreatedNotificationError(Context context, String creatorUsername, DomainMetadata targetDomainMetadata) {
        Intent intent = new Intent(context, MainActivity.class);
        Uri uri = Uri.parse(WebRoutes.creatorsIdAnnotationCollectionId(
                creatorUsername,
                Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT
        ));
        intent.setData(uri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);
        String descriptionText = targetDomainMetadata != null
                ? context.getString(R.string.source_created_error_notification_description, targetDomainMetadata.getUrl().getHost())
                : context.getString(R.string.source_created_error_notification_description_default);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.source_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.source_created_error_notification_title))
                .setStyle(
                        new NotificationCompat.BigTextStyle().bigText(descriptionText)
                )
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setAutoCancel(true);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(targetDomainMetadata.getUrl().getHost().hashCode(), builder.build());
    }

    public static void sourceCreatedNotificationStart(Context context, String creatorUsername, @NotNull DomainMetadata targetDomainMetadata, Pair<Integer, Integer> progress) {
        // FIXME: This should link to a source specific view, but none exists currently.
        Intent intent = new Intent(context, MainActivity.class);
        Uri uri = Uri.parse(WebRoutes.creatorsIdAnnotationCollectionId(
                creatorUsername,
                Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT
        ));
        intent.setData(uri);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, 0);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, context.getString(R.string.source_created_notification_channel_id))
                .setSmallIcon(R.drawable.ic_stat_name)
                .setColor(Color.BLACK)
                .setContentTitle(context.getString(R.string.source_created_start_notification_title))
                .setContentText(context.getString(R.string.source_created_start_notification_description, targetDomainMetadata.getUrl().getHost()))
                .setLargeIcon(targetDomainMetadata.getScaledFaviconWithBackground(context))
                .setPriority(NotificationCompat.PRIORITY_LOW)
                .setContentIntent(pendingIntent)
                .setProgress(progress.first, progress.second, false)
                .setOngoing(true);
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(context);

        notificationManager.notify(targetDomainMetadata.getUrl().getHost().hashCode(), builder.build());
    }
}
