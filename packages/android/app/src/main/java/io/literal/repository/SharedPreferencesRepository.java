package io.literal.repository;

import android.content.Context;
import android.content.SharedPreferences;

public class SharedPreferencesRepository {
    private static final String FILE_KEY = "io.literal.SHARED_PREFERENCES_REPOSITORY";

    private static final String IS_SIGNED_OUT_KEY = "IS_SIGNED_OUT";

    private static SharedPreferences getSharedPreferences(Context context) {
        return context.getSharedPreferences(FILE_KEY, Context.MODE_PRIVATE);
    }

    public static boolean getIsSignedOut(Context context) {
        return getSharedPreferences(context).getBoolean(IS_SIGNED_OUT_KEY, false);
    }
    public static void setIsSignedOut(Context context, boolean hasSignedOut) {
        SharedPreferences.Editor editor = getSharedPreferences(context).edit();
        editor.putBoolean(IS_SIGNED_OUT_KEY, hasSignedOut);
        editor.apply();
    }
}
