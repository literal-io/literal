package io.literal.service;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.app.Activity;
import android.view.View;
import android.view.Window;

import org.jetbrains.annotations.NotNull;

import io.literal.R;

public class SplashService {

    public static void hide(Activity activity, @NotNull View view) {
        int shortAnimationDuration = activity.getResources().getInteger(android.R.integer.config_shortAnimTime);
        view.animate()
                .alpha(0f)
                .setDuration(shortAnimationDuration)
                .setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        view.setVisibility(View.GONE);
                    }
                });
        setChromeColor(activity, R.color.colorDarkAccent);
    }

    public static boolean isVisible(@NotNull View view) {
        return view.getVisibility() == View.VISIBLE;
    }

    public static void show(Activity activity, @NotNull View view) {
        view.setVisibility(View.VISIBLE);
        setChromeColor(activity, R.color.colorPrimaryDark);
    }

    private static void setChromeColor(@NotNull Activity activity, int colorId) {
        Window window = activity.getWindow();
        int color = activity.getResources().getColor(colorId, activity.getTheme());

        window.setStatusBarColor(color);
        window.setNavigationBarColor(color);
    }
}
