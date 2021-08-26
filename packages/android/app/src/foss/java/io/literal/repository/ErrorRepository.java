package io.literal.repository;

import android.util.Log;

import java.util.Map;

import io.literal.lib.Thunk;
import io.literal.model.ErrorRepositoryLevel;

public class ErrorRepository {

    public static String CATEGORY_AUTHENTICATION = "Authentication";
    public static String CATEGORY_NAVIGATION = "Navigation";

    public static Thunk initialize() {
        Log.d("initialize", "");
        return () -> { /** noop **/ };
    }

    public static void captureException(Exception exception) {
        Log.d("captureException", "", exception);
    }

    public static void captureException(Throwable exception, Map<String, Object> context) {
        Log.d("captureException", context.toString(), exception);
    }

    public static void captureException(Throwable exception) {
        Log.d("captureException", "", exception);
    }

    public static void captureException(Exception exception, String message) {
        Log.d("captureException", message, exception);
    }

    public static void captureWarning(Exception exception) {
        Log.d("captureWarning", "", exception);
    }

    public static void captureWarning(Exception exception, Map<String, Object> context) {
        Log.d("captureWarning", context.toString(), exception);
    }

    public static void captureBreadcrumb(String category, String message, ErrorRepositoryLevel level) {
        Log.d("captureBreadcrumb", level.name() + ", " + category + ", " + message);
    }
}
