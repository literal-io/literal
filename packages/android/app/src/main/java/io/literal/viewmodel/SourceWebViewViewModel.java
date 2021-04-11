package io.literal.viewmodel;

import android.content.res.AssetManager;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Optional;

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
import io.literal.repository.ErrorRepository;

public class SourceWebViewViewModel extends ViewModel {
    private static final String GET_ANNOTATION_SCRIPT_NAME = "SourceWebViewGetAnnotation.js";
    private static final String ANNOTATION_RENDERER_SCRIPT_NAME = "SourceWebViewAnnotationRenderer.js";
    private static final String GET_ANNOTATION_BOUNDING_BOX_SCRIPT_NAME = "SourceWebViewGetAnnotationBoundingBox.js";
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> hasInjectedAnnotationRendererScript = new MutableLiveData<>(false);
    private final MutableLiveData<ArrayList<Annotation>> annotations = new MutableLiveData<>(new ArrayList<>());
    private ArrayList<String> createdAnnotationIds = new ArrayList<>();
    private final MutableLiveData<DomainMetadata> domainMetadata = new MutableLiveData<>(null);
    private final MutableLiveData<String> focusedAnnotationId = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayDeque<WebEvent>> webEvents = new MutableLiveData<>(null);
    private String getAnnotationScript = null;
    private String annotationRendererScript = null;
    private String getAnnotationBoundingBoxScript = null;

    public MutableLiveData<Boolean> getHasFinishedInitializing() {
        return hasFinishedInitializing;
    }

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public MutableLiveData<Boolean> getHasInjectedAnnotationRendererScript() {
        return hasInjectedAnnotationRendererScript;
    }

    public void setHasInjectedAnnotationRendererScript(boolean hasInjectedAnnotationRendererScript) {
        this.hasInjectedAnnotationRendererScript.setValue(hasInjectedAnnotationRendererScript);
    }

    public String getGetAnnotationScript(AssetManager assetManager) {
        if (getAnnotationScript == null) {
            try {
                getAnnotationScript =
                        IOUtils.toString(assetManager.open(GET_ANNOTATION_SCRIPT_NAME), StandardCharsets.UTF_8);
            } catch (IOException e) {
                ErrorRepository.captureException(e);
            }
        }
        return getAnnotationScript;
    }

    public String getAnnotationRendererScript(AssetManager assetManager, JSONArray paramAnnotations, String paramFocusedAnnotationId) {
        if (annotationRendererScript == null) {
            try {
                annotationRendererScript =
                        IOUtils.toString(assetManager.open(ANNOTATION_RENDERER_SCRIPT_NAME), StandardCharsets.UTF_8);
            } catch (IOException e) {
                ErrorRepository.captureException(e);
            }
        }

        String stringifiedParamAnnotations = JSONObject.quote(paramAnnotations.toString());
        String output = annotationRendererScript
                .replaceAll(
                        "process\\.env\\.PARAM_ANNOTATIONS",
                        stringifiedParamAnnotations.substring(1, stringifiedParamAnnotations.length() - 1)
                )
                .replaceAll(
                        "process\\.env\\.PARAM_FOCUSED_ANNOTATION_ID",
                        JSONObject.quote(paramFocusedAnnotationId)
                );
        return output;
    }

    public String getGetAnnotationBoundingBoxScript(AssetManager assetManager, JSONObject paramAnnotation) {
        if (getAnnotationBoundingBoxScript == null) {
            try {
                getAnnotationBoundingBoxScript = IOUtils.toString(assetManager.open(GET_ANNOTATION_BOUNDING_BOX_SCRIPT_NAME), StandardCharsets.UTF_8);
            } catch (IOException e) {
                ErrorRepository.captureException(e);
            }
        }
        String stringifiedParamAnnotation = JSONObject.quote(paramAnnotation.toString());
        return getAnnotationBoundingBoxScript
                .replaceAll(
                        "process\\.env\\.PARAM_ANNOTATION",
                        stringifiedParamAnnotation.substring(1, stringifiedParamAnnotation.length() - 1)
                );
    }

    public MutableLiveData<ArrayList<Annotation>> getAnnotations() {
        return annotations;
    }
    public void setAnnotations(ArrayList<Annotation> annotations) {
        this.annotations.setValue(annotations);
    }

    public MutableLiveData<DomainMetadata> getDomainMetadata() {
        return domainMetadata;
    }

    public void setDomainMetadata(DomainMetadata domainMetadata) {
        this.domainMetadata.setValue(domainMetadata);
    }

    public MutableLiveData<String> getFocusedAnnotationId() {
        return focusedAnnotationId;
    }

    public Optional<Annotation> getFocusedAnnotation() {
        if (focusedAnnotationId.getValue() == null) {
            return Optional.empty();
        }

        return annotations.getValue().stream()
                .filter(a -> a.getId().equals(focusedAnnotationId.getValue()))
                .findFirst();
    }

    public void setFocusedAnnotationId(String annotationId) {
        focusedAnnotationId.setValue(annotationId);
    }

    public ArrayList<String> getCreatedAnnotationIds() {
        return createdAnnotationIds;
    }

    public void setCreatedAnnotationIds(ArrayList<String> createdAnnotationIds) {
        this.createdAnnotationIds = createdAnnotationIds;
    }

    public Annotation createAnnotation(String json, String creatorUsername, boolean shouldAddAnnotation) {
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
                        annotation.getCreated(),
                        annotation.getModified(),
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
                        annotation.getCreated(),
                        annotation.getModified(),
                        annotationId
                );

            }

            if (shouldAddAnnotation) {
                newAnnotations.add(annotation);
                annotations.setValue(newAnnotations);
                createdAnnotationIds.add(annotation.getId());
            }
            return annotation;
        } catch (Exception e) {
            ErrorRepository.captureException(e);
            return null;
        }
    }

    public void createAnnotation(Annotation annotation) {
        ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
        newAnnotations.add(annotation);
        annotations.setValue(newAnnotations);
        createdAnnotationIds.add(annotation.getId());
    }

    public void addAnnotation(Annotation annotation) {
        ArrayList<Annotation> newAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
        newAnnotations.add(annotation);
        annotations.setValue(newAnnotations);
    }

    public boolean updateAnnotation(Annotation annotation) {
        if (annotation.getId() != null) {
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

    public boolean removeAnnotation(String annotationId) {
        ArrayList<Annotation> updatedAnnotations = (ArrayList<Annotation>) annotations.getValue().clone();
        boolean updated = updatedAnnotations.removeIf((annotation) -> annotation.getId().equals(annotationId));
        if (updated) {
            annotations.setValue(updatedAnnotations);
        }
        return updated;
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
