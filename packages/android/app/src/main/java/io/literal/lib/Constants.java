package io.literal.lib;

import io.literal.BuildConfig;

public class Constants {
    public static final String LOG_TAG = "Literal";

    public static final String WEB_HOST = BuildConfig.DEBUG ? "http://localhost:3000" : "https://literal.io";

    public static final String NOTIFICATION_CHANNEL_NOTE_CREATED_ID = "NOTE_CREATED";
    public static final String NOTIFICATION_CHANNEL_NOTE_CREATED_NAME = "Note created";
    public static final String NOTIFICATION_CHANNEL_NOTE_CREATED_DESCRIPTION = "Confirmation that a note has been created.";
    public static final String NOTIFICATION_NOTE_CREATED_TITLE = "Note Created";
}
