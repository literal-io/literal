package io.literal.repository;

import android.app.Activity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;
import android.widget.Toast;

import io.literal.R;

public class ToastRepository {

    public static int STYLE_DARK_PRIMARY = 0;
    public static int STYLE_DARK_ACCENT = 1;

    public static void show(Activity activity, int resourceId, int style) {
        LayoutInflater inflater = activity.getLayoutInflater();

        View layout = inflater.inflate(
                style == STYLE_DARK_PRIMARY ? R.layout.toast_primary : R.layout.toast_accent,
                activity.findViewById(R.id.custom_toast_container)
        );
        TextView textView = layout.findViewById(R.id.text);
        textView.setText(activity.getResources().getText(resourceId));

        Toast toast = new Toast(activity.getApplicationContext());
        toast.setDuration(Toast.LENGTH_LONG);
        toast.setView(layout);
        toast.show();
    }

    public static void show(Activity activity, int textResourceId) {
        ToastRepository.show(activity, textResourceId, STYLE_DARK_PRIMARY);
    }
}
