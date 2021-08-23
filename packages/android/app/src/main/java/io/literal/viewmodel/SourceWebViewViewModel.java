package io.literal.viewmodel;

import android.content.Context;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.json.JSONObject;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;

import io.literal.lib.AnnotationCollectionLib;
import io.literal.lib.AnnotationLib;
import io.literal.lib.ArrayUtil;
import io.literal.lib.Constants;
import io.literal.lib.Crypto;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.Body;
import io.literal.model.Format;
import io.literal.model.Language;
import io.literal.model.Motivation;
import io.literal.model.SourceWebViewAnnotation;
import io.literal.model.Target;
import io.literal.model.TextDirection;
import io.literal.model.TextualBody;
import io.literal.model.TextualTarget;
import io.literal.model.User;
import io.literal.model.WebArchive;
import io.literal.repository.ErrorRepository;
import io.literal.ui.view.SourceWebView.Source;
import io.literal.ui.view.SourceWebView.SourceWebView;

public class SourceWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> sourceHasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<Boolean> sourceJavascriptEnabled = new MutableLiveData<>(true);

    private final HashMap<String, SourceWebViewAnnotation> annotations = new HashMap<>();
    private final HashMap<String, CompletableFuture<Annotation>> compiledAnnotations = new HashMap<>();
    private final MutableLiveData<SourceWebViewAnnotation[]> annotationsLiveData = new MutableLiveData<>(new SourceWebViewAnnotation[0]);

    public MutableLiveData<SourceWebViewAnnotation[]> getAnnotations() {
        return annotationsLiveData;
    }

    public MutableLiveData<Boolean> getSourceHasFinishedInitializing() {
        return sourceHasFinishedInitializing;
    }

    public void setSourceHasFinishedInitializing(boolean sourceHasFinishedInitializing) {
        this.sourceHasFinishedInitializing.setValue(sourceHasFinishedInitializing);
    }

    public MutableLiveData<Boolean> getSourceJavascriptEnabled() {
        return sourceJavascriptEnabled;
    }

    public void setSourceJavascriptEnabled(boolean sourceJavascriptEnabled) {
        this.sourceJavascriptEnabled.setValue(sourceJavascriptEnabled);
    }

    public void reset() {
        this.annotations.clear();
        this.compiledAnnotations.clear();
        this.sourceHasFinishedInitializing.setValue(false);
        this.annotationsLiveData.setValue(new SourceWebViewAnnotation[0]);
    }

    public Optional<SourceWebViewAnnotation> getFocusedAnnotation() {
        return this.annotations.values().stream()
                .filter((annotation) -> annotation.getFocusStatus().equals(SourceWebViewAnnotation.FocusStatus.FOCUSED))
                .findFirst();
    }

    public Optional<SourceWebViewAnnotation> getAnnotation(String annotationId) {
        return Optional.ofNullable(this.annotations.get(annotationId));
    }

    public void setFocusedAnnotationId(String annotationId) {
        annotations.values().forEach((sourceWebViewAnnotation -> sourceWebViewAnnotation.setFocusStatus(SourceWebViewAnnotation.FocusStatus.NOT_FOCUSED)));
        Optional.ofNullable(annotations.get(annotationId)).ifPresent((sourceWebViewAnnotation -> sourceWebViewAnnotation.setFocusStatus(SourceWebViewAnnotation.FocusStatus.FOCUSED)));
    }

    public SourceWebViewAnnotation createAnnotation(String json, String creatorUsername, WebArchive webArchive) {
        try {
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

            SourceWebViewAnnotation sourceWebViewAnnotation = new SourceWebViewAnnotation(
                    annotation,
                    webArchive,
                    SourceWebViewAnnotation.CreationStatus.REQUIRES_CREATION,
                    SourceWebViewAnnotation.FocusStatus.NOT_FOCUSED
            );
            annotations.put(annotation.getId(), sourceWebViewAnnotation);
            annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));

            return sourceWebViewAnnotation;
        } catch (Exception e) {
            ErrorRepository.captureException(e);
            return null;
        }
    }

    public void createAnnotation(Annotation annotation) {
        annotations.put(
                annotation.getId(),
                new SourceWebViewAnnotation(
                        annotation,
                        null,
                        SourceWebViewAnnotation.CreationStatus.REQUIRES_CREATION,
                        SourceWebViewAnnotation.FocusStatus.NOT_FOCUSED
                )
        );
        annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));
    }

    public void addAnnotation(Annotation annotation, boolean focusAnnotation) {
        annotations.put(
                annotation.getId(),
                new SourceWebViewAnnotation(
                        annotation,
                        null,
                        SourceWebViewAnnotation.CreationStatus.CREATED,
                        focusAnnotation ? SourceWebViewAnnotation.FocusStatus.FOCUSED : SourceWebViewAnnotation.FocusStatus.NOT_FOCUSED
                )
        );
        annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));
    }

    public void addAnnotations(Annotation[] annotationsToAdd) {
        Arrays.stream(annotationsToAdd).forEach((annotation) -> {
            annotations.put(
                    annotation.getId(),
                    new SourceWebViewAnnotation(
                            annotation,
                            null,
                            SourceWebViewAnnotation.CreationStatus.CREATED,
                            SourceWebViewAnnotation.FocusStatus.NOT_FOCUSED
                    )
            );
        });
        annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));
    }

    public boolean updateAnnotation(@NonNull Annotation annotation) {
        if (annotation.getId() == null) {
            return false;
        }

        Optional<SourceWebViewAnnotation> sourceWebViewAnnotation = Optional.ofNullable(annotations.get(annotation.getId()));
        if (sourceWebViewAnnotation.isPresent()) {
            sourceWebViewAnnotation.get().setAnnotation(annotation);
            annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));
        }

        return sourceWebViewAnnotation.isPresent();
    }

    public boolean removeAnnotation(String annotationId) {
        boolean didRemove = Optional.ofNullable(annotations.remove(annotationId)).isPresent();
        if (didRemove) {
            annotationsLiveData.setValue(annotations.values().toArray(new SourceWebViewAnnotation[0]));
        }
        return didRemove;
    }

    public Optional<CompletableFuture<Annotation>> compileAnnotation(Context context, User user, Executor executor, String annotationId) {
        if (!compiledAnnotations.containsKey(annotationId)) {
            Optional<SourceWebViewAnnotation> annotation = Optional.ofNullable(annotations.get(annotationId));
            if (!annotation.isPresent()) {
                ErrorRepository.captureWarning(new Exception("Tried to compile annotation where no SourceWebViewAnnotation exists."));
                return Optional.empty();
            }

            CompletableFuture<Annotation> compiledAnnotation = annotation.get().compileWebArchive(context, user, executor);
            compiledAnnotations.put(annotationId, compiledAnnotation);
        }

        return Optional.ofNullable(compiledAnnotations.get(annotationId));
    }

    public Optional<CompletableFuture<Annotation>> getCompiledAnnotation(String annotationId) {
        return Optional.ofNullable(compiledAnnotations.get(annotationId));
    }
}
