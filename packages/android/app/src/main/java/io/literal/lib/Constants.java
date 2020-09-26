package io.literal.lib;

import io.literal.BuildConfig;

public class Constants {
    public static final String LOG_TAG = "Literal";

    public static final String WEB_HOST = BuildConfig.DEBUG ? "http://localhost:3000" : "https://literal.io";

    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID = "ANNOTATION_CREATED";
    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_NAME = "Annotation created";
    public static final String NOTIFICATION_CHANNEL_ANNOTATION_CREATED_DESCRIPTION = "Confirmation that an annotation has been created.";
    public static final String NOTIFICATION_ANNOTATION_CREATED_TITLE = "Annotation Created";

    public static final String RECENT_ANNOTATION_COLLECTION_ID_COMPONENT = "034a7e52c5c9534b709dc1dba403868399b0949f7c1933a67325c22077ffc221";
    public static final String RECENT_ANNOTATION_COLLECTION_LABEL = "recent";
}
