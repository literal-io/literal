package io.literal.lib;

import io.literal.BuildConfig;

public class Constants {
    public static final String LOG_TAG = "Literal";

    public static final String WEB_HOST = BuildConfig.DEBUG ? "http://localhost:3000" : "https://literal.io";

    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID = "ANNOTATION_CREATED";
    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_NAME = "Annotation created";
    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_DESCRIPTION = "Confirmation that an annotation has been created.";
    public static final String NOTIFICATION_ANNOTATION_CREATED_TITLE = "Annotation Created";
}
