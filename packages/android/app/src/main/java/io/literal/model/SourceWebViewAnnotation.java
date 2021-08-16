package io.literal.model;

import android.content.Context;
import android.util.Log;

import java.net.URI;
import java.util.Arrays;
import java.util.Date;
import java.util.concurrent.CompletableFuture;

import io.literal.lib.AmazonS3URILib;
import io.literal.lib.DateUtil;
import io.literal.ui.view.SourceWebView.Source;

public class SourceWebViewAnnotation {
    private Annotation annotation;
    private WebArchive webArchive;
    private CreationStatus creationStatus;
    private FocusStatus focusStatus;

    public SourceWebViewAnnotation(Annotation annotation, WebArchive webArchive, CreationStatus creationStatus, FocusStatus focusStatus) {
        this.annotation = annotation;
        this.creationStatus = creationStatus;
        this.focusStatus = focusStatus;
        this.webArchive = webArchive;
    }

    public CompletableFuture<Void> compileWebArchive(Context context, User user) {
        return webArchive.compile(context, user)
                .thenApply((updatedWebArchive) -> {
                    if (this.webArchive != updatedWebArchive) {
                        this.annotation = Arrays.stream(annotation.getTarget())
                                .filter(t -> t instanceof SpecificTarget)
                                .findFirst()
                                .map((specificTarget) -> {
                                    SpecificTarget updatedSpecifcTarget = new SpecificTarget.Builder((SpecificTarget) specificTarget)
                                            .setState(new State[]{
                                                    new TimeState(
                                                            new URI[]{updatedWebArchive.getStorageObject().getCanonicalURI(context, user)},
                                                            new String[]{DateUtil.toISO8601UTC(new Date())}
                                                    )
                                            })
                                            .build();

                                    return annotation.updateTarget(updatedSpecifcTarget);
                                })
                                .orElse(annotation);
                        this.webArchive = updatedWebArchive;
                    }
                    return null;
                });
    }

    public FocusStatus getFocusStatus() {
        return focusStatus;
    }

    public void setFocusStatus(FocusStatus focusStatus) {
        this.focusStatus = focusStatus;
    }

    public Annotation getAnnotation() {
        return annotation;
    }

    public void setAnnotation(Annotation annotation) {
        this.annotation = annotation;
    }

    public CreationStatus getCreationStatus() {
        return creationStatus;
    }

    public void setCreationStatus(CreationStatus creationStatus) {
        this.creationStatus = creationStatus;
    }

    public WebArchive getWebArchive() {
        return webArchive;
    }

    public void setWebArchive(WebArchive webArchive) {
        this.webArchive = webArchive;
    }


    public enum CreationStatus {
        REQUIRES_CREATION,
        CREATED;
    }

    public enum FocusStatus {
        FOCUSED,
        NOT_FOCUSED;
    }
}
