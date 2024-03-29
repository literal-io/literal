package io.literal.service;

import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.os.Parcelable;
import android.util.Log;

import androidx.annotation.NonNull;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;
import java.util.Optional;
import java.util.UUID;
import java.util.stream.Collectors;

import io.literal.lib.JsonArrayUtil;
import io.literal.model.Annotation;
import io.literal.model.WebArchive;
import io.literal.repository.ErrorRepository;

public class CreateAnnotationIntent {
    public static String ACTION = "ACTION_CREATE_ANNOTATIONS";
    public static String EXTRA_ID = "EXTRA_ID";
    public static String EXTRA_ANNOTATION = "EXTRA_ANNOTATION";
    public static String EXTRA_FAVICON = "EXTRA_FAVICON";
    public static String EXTRA_DISPLAY_URI = "EXTRA_DISPLAY_URI";
    public static String EXTRA_DISABLE_NOTIFICATION = "EXTRA_DISABLE_NOTIFICATION";

    private final Annotation annotation;
    private final Optional<File> favicon;
    private final Optional<URI> displayUri;
    private final String id;
    private final boolean disableNotification;

    private Optional<Bitmap> faviconBitmap;

    public CreateAnnotationIntent(
            @NonNull Annotation annotation,
            @NonNull Optional<File> favicon,
            @NonNull Optional<URI> displayUri,
            @NonNull boolean disableNotification,
            @NonNull String id
    ) {
        this.annotation = annotation;
        this.favicon = favicon;
        this.id = id;
        this.displayUri = displayUri;
        this.disableNotification = disableNotification;
        this.faviconBitmap = Optional.empty();
    }

    public String getId() {
        return id;
    }

    public Annotation getAnnotation() {
        return annotation;
    }

    public Optional<File> getFavicon() {
        return favicon;
    }

    public Optional<URI> getDisplayURI() {
        return displayUri;
    }

    public Optional<Bitmap> getFaviconBitmap() {
        if (!faviconBitmap.isPresent() && favicon.isPresent()) {
            faviconBitmap = Optional.ofNullable(BitmapFactory.decodeFile(favicon.get().getPath()));
        }
        return faviconBitmap;
    }

    public static Optional<CreateAnnotationIntent> fromIntent(Context context, Intent intent) {
        if (!intent.getAction().equals(ACTION)) {
            return Optional.empty();
        }

        String extraAnnotation = intent.getStringExtra(EXTRA_ANNOTATION);
        String extraId = intent.getStringExtra(EXTRA_ID);
        boolean extraDisableNotification = intent.getBooleanExtra(EXTRA_DISABLE_NOTIFICATION, false);

        Optional<String> extraFavicon = Optional.ofNullable(intent.getStringExtra(EXTRA_FAVICON));
        Optional<String> extraDisplayURI = Optional.ofNullable(intent.getStringExtra(EXTRA_DISPLAY_URI));

        try {
            Annotation annotation = Annotation.fromJson(new JSONObject(extraAnnotation));
            Optional<File> favicon = extraFavicon.map(File::new);
            Optional<URI> displayURI = extraDisplayURI.flatMap(u -> {
                try {
                    return Optional.of(new URI(u));
                } catch (URISyntaxException e) {
                    ErrorRepository.captureException(e);
                    return Optional.empty();
                }
            });

            CreateAnnotationIntent.Builder builder = new CreateAnnotationIntent.Builder();
            return Optional.of(
                    builder
                            .setAnnotation(annotation)
                            .setFavicon(favicon)
                            .setDisplayURI(displayURI)
                            .setId(extraId)
                            .setDisableNotification(extraDisableNotification)
                            .build()
            );
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
            return Optional.empty();
        }
    }

    public Optional<Intent> toIntent(Context context) {
        try {
            Intent serviceIntent = new Intent(context, AnnotationService.class);
            serviceIntent.setAction(ACTION);
            serviceIntent.putExtra(EXTRA_ID, id);
            serviceIntent.putExtra(EXTRA_ANNOTATION, annotation.toJson().toString());
            serviceIntent.putExtra(EXTRA_DISABLE_NOTIFICATION, disableNotification);

            favicon.ifPresent((f) -> serviceIntent.putExtra(EXTRA_FAVICON, f.getAbsolutePath()));
            displayUri.ifPresent(d -> serviceIntent.putExtra(EXTRA_DISPLAY_URI, d.toString()));

            return Optional.of(serviceIntent);
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
            return Optional.empty();
        }
    }

    public JSONObject toJSON() {
        try {
            JSONObject data = new JSONObject();
            data.put("action", ACTION);
            data.put(EXTRA_ID, id);
            data.put(EXTRA_ANNOTATION, annotation.toJson());
            data.put(EXTRA_DISABLE_NOTIFICATION, disableNotification);

            favicon.ifPresent((f) -> {
                try {
                    data.put(EXTRA_FAVICON, f.getAbsolutePath());
                } catch (JSONException ignored) {}
            });
            displayUri.ifPresent(d -> {
                try {
                    data.put(EXTRA_DISPLAY_URI, d.toString());
                } catch (JSONException ignored) {}
            });

            return data;
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
            return null;
        }
    }

    public boolean getDisableNotification() {
        return disableNotification;
    }

    public static class Builder {
        private Annotation annotation;
        private Optional<File> favicon = Optional.empty();
        private Optional<URI> displayURI = Optional.empty();
        private boolean disableNotification = false;
        private String id;

        public Builder() {
        }

        public Builder setAnnotation(Annotation annotation) {
            this.annotation = annotation;
            return this;
        }

        public Builder setFavicon(Optional<File> favicon) {
            this.favicon = favicon;
            return this;
        }

        public Builder setId(String id) {
            this.id = id;
            return this;
        }

        public Builder setDisplayURI(Optional<URI> displayURI) {
            this.displayURI = displayURI;
            return this;
        }


        public Builder setDisableNotification(boolean disableNotification) {
            this.disableNotification = disableNotification;
            return this;
        }

        public CreateAnnotationIntent build() {
            Objects.requireNonNull(annotation);
            Objects.requireNonNull(id);

            return new CreateAnnotationIntent(
                    annotation,
                    favicon,
                    displayURI,
                    disableNotification,
                    id
            );
        }
    }
}
