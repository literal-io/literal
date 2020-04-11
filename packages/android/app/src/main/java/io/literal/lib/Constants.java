package io.literal.lib;


import io.literal.BuildConfig;

public class Constants {
    public static final String WEB_HOST = BuildConfig.DEBUG ? "http://localhost:3000" : "https://literal.io";
}
