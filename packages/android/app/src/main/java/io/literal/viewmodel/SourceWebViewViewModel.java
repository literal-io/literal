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
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;

import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.AnnotationLib;
import io.literal.lib.ArrayUtil;
import io.literal.lib.Constants;
import io.literal.lib.Crypto;
import io.literal.lib.DomainMetadata;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.Body;
import io.literal.model.Format;
import io.literal.model.Language;
import io.literal.model.Motivation;
import io.literal.model.Target;
import io.literal.model.TextDirection;
import io.literal.model.TextualBody;
import io.literal.model.TextualTarget;

public class SourceWebViewViewModel extends ViewModel {
    private static final String GET_ANNOTATION_SCRIPT_NAME = "SourceWebViewGetAnnotation.js";
    private static final String HIGHLIGHT_ANNOTATION_TARGET_SCRIPT_NAME = "SourceWebViewHighlightAnnotationTarget.js";
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<String> getSelectorScript = new MutableLiveData<>(null);
    private final MutableLiveData<String> highlightAnnotationTargetScript = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayList<Annotation>> annotations = new MutableLiveData<>(new ArrayList<>());
    private final MutableLiveData<DomainMetadata> domainMetadata = new MutableLiveData<>(null);
    private final MutableLiveData<Annotation> focusedAnnotation = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayDeque<WebEvent>> webEvents = new MutableLiveData<>(null);

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

        String stringifiedParamAnnotations = JSONObject.quote(paramAnnotations.toString());
        return highlightAnnotationTargetScript.getValue().replaceAll("\\$\\{PARAM_ANNOTATIONS\\}", stringifiedParamAnnotations.substring(1, stringifiedParamAnnotations.length() - 1));
    }

    public MutableLiveData<ArrayList<Annotation>> getAnnotations() {
        return annotations;
    }

    public MutableLiveData<DomainMetadata> getDomainMetadata() {
        return domainMetadata;
    }

    public MutableLiveData<Annotation> getFocusedAnnotation() {
        return focusedAnnotation;
    }


    public void setFocusedAnnotation(Annotation annotation) {
        focusedAnnotation.setValue(annotation);
    }

    public Annotation createAnnotation(String json, String creatorUsername) {
        try {
            ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
            Annotation annotation = Annotation.fromJson(new JSONObject(json));

            boolean needsRecentTag =
                    Arrays.stream(annotation.getBody() == null ? new Body[0] : annotation.getBody())
                            .noneMatch((body) -> {
                                if (body.getType() == Body.Type.TEXTUAL_BODY) {
                                    TextualBody textualBody = (TextualBody) body;
                                    return AnnotationLib.idComponentFromId(textualBody.getId()).equals(Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT);
                                }
                                return false;
                            });
            if (needsRecentTag) {
                Body recentTag = new TextualBody(
                        AnnotationCollectionLib.makeId(
                                creatorUsername,
                                Constants.RECENT_ANNOTATION_COLLECTION_LABEL
                        ),
                        Format.TEXT_PLAIN,
                        Language.EN_US,
                        Language.EN_US,
                        TextDirection.LTR,
                        null,
                        null,
                        new Motivation[]{Motivation.TAGGING},
                        Constants.RECENT_ANNOTATION_COLLECTION_LABEL
                );

                annotation = new Annotation(
                        ArrayUtil.add(
                                annotation.getBody() != null ? annotation.getBody() : new Body[0],
                                recentTag
                        ),
                        annotation.getTarget(),
                        annotation.getMotivation(),
                        annotation.getId()
                );
            }

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

                annotation = new Annotation(
                        annotation.getBody(),
                        annotation.getTarget(),
                        annotation.getMotivation(),
                        annotationId
                );

            }

            newAnnotations.add(annotation);
            annotations.setValue(newAnnotations);
            return annotation;
        } catch (Exception e) {
            Log.d("SourceWebViewViewModel", "createAnnotation", e);
            return null;
        }
    }

    public boolean updateAnnotation(Annotation annotation) {
        if (annotation.getId() != null) {

            if (focusedAnnotation.getValue() != null && focusedAnnotation.getValue().getId().equals(annotation.getId())) {
                focusedAnnotation.setValue(annotation);
            }

            ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
            int idx = -1;
            for (int i = 0; i < newAnnotations.size(); i++) {
                if (newAnnotations.get(i).getId().equals(annotation.getId())) {
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

    public void dispatchWebEvent(WebEvent webEvent) {
        ArrayDeque<WebEvent> newWebEvents;
        if (webEvents.getValue() == null) {
            newWebEvents = new ArrayDeque<>();
        } else {
            newWebEvents = webEvents.getValue().clone();
        }
        newWebEvents.add(webEvent);
        webEvents.setValue(newWebEvents);
    }

    public void clearWebEvents() {
        webEvents.setValue(null);
    }

    public MutableLiveData<ArrayDeque<WebEvent>> getWebEvents() {
        return webEvents;
    }
}
