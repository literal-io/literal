package io.literal.viewmodel;

import android.content.res.AssetManager;
import android.graphics.Bitmap;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Arrays;

import io.literal.lib.Crypto;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.TextualTarget;
import io.literal.model.Target;

public class SourceWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<String> getSelectorScript = new MutableLiveData<>(null);
    private final MutableLiveData<String> highlightAnnotationTargetScript = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayList<Annotation>> annotations = new MutableLiveData<>(new ArrayList<>());
    private final MutableLiveData<DomainMetadata> domainMetadata = new MutableLiveData<>(null);
    private final MutableLiveData<Annotation> focusedAnnotation = new MutableLiveData<>(null);

    private static final String GET_ANNOTATION_SCRIPT_NAME = "SourceWebViewGetAnnotation.js";
    private static final String HIGHLIGHT_ANNOTATION_TARGET_SCRIPT_NAME = "SourceWebViewHighlightAnnotationTarget.js";

    public MutableLiveData<Boolean> getHasFinishedInitializing() {
        return hasFinishedInitializing;
    }

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public void setDomainMetadata(URL url, Bitmap favicon) {
        this.domainMetadata.setValue(new DomainMetadata(url, favicon));
    }

    public String getGetAnnotationScript(AssetManager assetManager) {
        if (getSelectorScript.getValue() == null) {
            try {
                getSelectorScript.setValue(
                        IOUtils.toString(assetManager.open(GET_ANNOTATION_SCRIPT_NAME), StandardCharsets.UTF_8)
                );
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "getGetSelectorScript", e);
            }
        }
        return getSelectorScript.getValue();
    }

    public String getHighlightAnnotationTargetScript(AssetManager assetManager, JSONArray paramAnnotations) {
        if (highlightAnnotationTargetScript.getValue() == null) {
            try {
                highlightAnnotationTargetScript.setValue(
                        IOUtils.toString(assetManager.open(HIGHLIGHT_ANNOTATION_TARGET_SCRIPT_NAME), StandardCharsets.UTF_8)
                );
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "getHighlightAnnotationTargetScript", e);
            }
        }

        return highlightAnnotationTargetScript.getValue().replaceAll("\\$\\{PARAM_ANNOTATIONS\\}", paramAnnotations.toString());
    }

    public MutableLiveData<ArrayList<Annotation>> getAnnotations() {
        return annotations;
    }

    public MutableLiveData<DomainMetadata> getDomainMetadata() {
        return domainMetadata;
    }

    public MutableLiveData<Annotation> getFocusedAnnotation() { return focusedAnnotation; };
    public void setFocusedAnnotation(Annotation annotation) {
        focusedAnnotation.setValue(annotation);
    }

    public Annotation createAnnotation(String json, String creatorUsername) {
        try {
            ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
            Annotation annotation = Annotation.fromJson(new JSONObject(json));
            if (annotation.getId() == null) {
                TextualTarget textualTarget = (TextualTarget)
                        Arrays.stream(annotation.getTarget()).filter(target -> target.getType() == Target.Type.TEXTUAL_TARGET)
                        .findFirst()
                        .get();
                String valueHash = Crypto.sha256Hex(textualTarget.getValue());
                String annotationId = WebRoutes.creatorsIdAnnotationId(
                        WebRoutes.getAPIHost(),
                        creatorUsername,
                        valueHash
                );
                Annotation annotationWithId = new Annotation(
                        annotation.getBody(),
                        annotation.getTarget(),
                        annotationId
                );
                newAnnotations.add(annotationWithId);
                annotations.setValue(newAnnotations);
                return annotationWithId;
            } else {
                newAnnotations.add(annotation);
                annotations.setValue(newAnnotations);
                return annotation;
            }
        } catch (Exception e) {
            Log.d("SourceWebViewViewModel", "createAnnotation", e);
            return null;
        }
    }

    public boolean updateAnnotation(Annotation annotation) {
        if (annotation.getId() != null) {
            ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
            int idx = -1;
            for (int i = 0; i < newAnnotations.size(); i++) {
                if (newAnnotations.get(i).getId() == annotation.getId()) {
                    idx = i;
                    break;
                }
            }

            if (idx != -1) {
                newAnnotations.set(idx, annotation);
                annotations.setValue(newAnnotations);
                return true;
            }
        }
        return false;
    }

    public class DomainMetadata {
        private final URL url;
        private final Bitmap favicon;

        public DomainMetadata(URL url, Bitmap favicon) {
            this.url = url;
            this.favicon = favicon;
        }

        public Bitmap getFavicon() {
            return favicon;
        }

        public URL getUrl() {
            return url;
        }
    }
}
