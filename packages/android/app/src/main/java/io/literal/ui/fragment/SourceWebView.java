package io.literal.ui.fragment;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.os.Bundle;
import android.os.SystemClock;
import android.util.Log;
import android.view.ActionMode;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.webkit.ValueCallback;
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.services.s3.AmazonS3URI;
import com.google.android.gms.auth.api.signin.internal.Storage;
import com.google.android.material.appbar.AppBarLayout;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.net.MalformedURLException;
import java.net.URL;
import java.time.Instant;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.Optional;
import java.util.UUID;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Callback;
import io.literal.lib.DateUtil;
import io.literal.lib.DomainMetadata;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.WebEvent;
import io.literal.model.Annotation;
import io.literal.model.ExternalTarget;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.Target;
import io.literal.model.TextualTarget;
import io.literal.model.TimeState;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ArchiveRepository;
import io.literal.repository.NotificationRepository;
import io.literal.repository.ShareTargetHandlerRepository;
import io.literal.repository.StorageRepository;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;
import type.DeleteAnnotationInput;
import type.PatchAnnotationInput;
import type.PatchAnnotationOperationInput;

public class SourceWebView extends Fragment {

    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";
    private static final String PARAM_APP_WEB_VIEW_MODEL_KEY = "PARAM_APP_WEB_VIEW_MODEL_KEY";
    private static final String PARAM_TOOLBAR_PRIMARY_ACTION_ICON_RESOURCE_ID = "PARAM_PRIMARY_TOOLBAR_ACTION_ICON_RESOURCE_ID";

    private String paramInitialUrl;
    private String paramAppWebViewModelKey;
    private int paramToolbarPrimaryActionResourceId;

    private io.literal.ui.view.SourceWebView webView;
    private Toolbar toolbar;
    private AppBarLayout appBarLayout;
    private Callback<Void, Annotation[]> onToolbarPrimaryActionCallback;

    private ActionMode editAnnotationActionMode;
    /**
     * Show a different CAB if text is selected while editing annotation
     **/
    private boolean isEditingAnnotation;

    private boolean shouldClearHistoryOnPageFinished;

    private SourceWebViewViewModel sourceWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private AppWebViewViewModel appWebViewViewModel;

    public static SourceWebView newInstance(String initialUrl, String appWebViewModelKey, int toolbarPrimaryActionResourceId) {
        SourceWebView fragment = new SourceWebView();
        Bundle args = new Bundle();
        args.putString(PARAM_INITIAL_URL, initialUrl);
        args.putString(PARAM_APP_WEB_VIEW_MODEL_KEY, appWebViewModelKey);
        args.putInt(PARAM_TOOLBAR_PRIMARY_ACTION_ICON_RESOURCE_ID, toolbarPrimaryActionResourceId);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            paramInitialUrl = getArguments().getString(PARAM_INITIAL_URL);
            paramAppWebViewModelKey = getArguments().getString(PARAM_APP_WEB_VIEW_MODEL_KEY);
            paramToolbarPrimaryActionResourceId = getArguments().getInt(PARAM_TOOLBAR_PRIMARY_ACTION_ICON_RESOURCE_ID);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View v = inflater.inflate(R.layout.fragment_source_web_view, container, false);
        setHasOptionsMenu(true);
        return v;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        sourceWebViewViewModel = new ViewModelProvider(requireActivity()).get(SourceWebViewViewModel.class);
        authenticationViewModel = new ViewModelProvider(requireActivity()).get(AuthenticationViewModel.class);
        appWebViewViewModel =
                paramAppWebViewModelKey != null
                        ? new ViewModelProvider(requireActivity()).get(paramAppWebViewModelKey, AppWebViewViewModel.class)
                        : new ViewModelProvider(requireActivity()).get(AppWebViewViewModel.class);

        toolbar = view.findViewById(R.id.toolbar);
        ((AppCompatActivity) getActivity()).setSupportActionBar(toolbar);

        appBarLayout = view.findViewById(R.id.app_bar_layout);

        webView = view.findViewById(R.id.source_web_view);
        webView.setOnReceivedIcon((e, webView, icon) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnReceivedIcon callback error:", e);
            }

            DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            sourceWebViewViewModel.setDomainMetadata(domainMetadata != null ? domainMetadata.getUrl() : null, icon);
        });
        webView.setOnGetWebMessageChannelInitializerScript((_e, _data) -> {
            sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(true);
            return getAnnotationRendererScript();
        });

        webView.setExternalWebViewClient(new WebViewClient() {
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
                try {
                    sourceWebViewViewModel.setDomainMetadata(new URL(url), domainMetadata != null ? domainMetadata.getFavicon() : null);
                } catch (MalformedURLException ex) {
                    Log.d("SourceWebView", "Unable to execute setDomainMetadata:", ex);
                }
                sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(false);
            }

            @Override
            public void onPageFinished(WebView view, String url) {

                Log.i("SourceWebView", "onPageFinished: " + url);
                if (!sourceWebViewViewModel.getHasInjectedAnnotationRendererScript().getValue()) {
                    sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(true);
                    webView.evaluateJavascript(getAnnotationRendererScript(), _v -> { /** noop **/});
                }

                if (shouldClearHistoryOnPageFinished) {
                    view.clearHistory();
                }
            }
        });

        webView.setOnAnnotationCreated((e, _view) -> {
            if (e != null) {
                Log.d("SourceWebView", "setOnAnnotationCreated callback error:", e);
                return;
            }
            this.handleAnnotationCreated();

        });
        webView.setOnAnnotationCancelEdit((e, _view) -> {
            this.handleAnnotationCancelEdit();
        });
        webView.setOnAnnotationCommitEdit((e, _view) -> {
            this.handleAnnotationCommitEdit();
        });
        webView.setOnGetTextSelectionMenu((e, data) -> isEditingAnnotation
                ? R.menu.source_webview_commit_edit_annotation_menu
                : R.menu.source_webview_create_annotation_menu);
        webView.setOnDestroyTextSelectionMenu((e, actionMode) -> {
            if (isEditingAnnotation) {
                this.handleAnnotationCancelEdit();
            }
        });

        sourceWebViewViewModel.getAnnotations().observe(getActivity(), (annotations) -> {
            if (this.webView == null) {
                Log.d("SourceWebView", "Expected webView, but found none.");
                return;
            }

            this.handleRenderAnnotations(
                    annotations,
                    sourceWebViewViewModel.getFocusedAnnotation().getValue()
            );
        });

        sourceWebViewViewModel.getFocusedAnnotation().observe(getActivity(), this::dispatchFocusAnnotationWebEvent);

        sourceWebViewViewModel.getDomainMetadata().observe(getActivity(), (domainMetadata) -> {
            if (domainMetadata != null) {
                toolbar.setTitle(domainMetadata.getUrl().getHost());
                Bitmap favicon = domainMetadata.getFavicon();
                if (favicon != null) {

                    toolbar.setLogo(new BitmapDrawable(getResources(), domainMetadata.getScaledFaviconWithBackground(getContext())));
                }
            }
        });
        webView.setWebEventCallback((e, webView, event) -> {
            switch (event.getType()) {
                case WebEvent.TYPE_FOCUS_ANNOTATION:
                    try {
                        String annotationId = event.getData().getString("annotationId");
                        JSONObject boundingBox = event.getData().getJSONObject("boundingBox");
                        Rect annotationBoundingBox = new Rect(
                                boundingBox.getInt("left"),
                                boundingBox.getInt("top"),
                                boundingBox.getInt("right"),
                                boundingBox.getInt("bottom")
                        );

                        handleAnnotationFocus(annotationId, annotationBoundingBox);
                    } catch (JSONException ex) {
                        Log.d("SourceWebView", "Unable to parse FOCUS_ANNOTATION data: " + event.getData(), e);
                    }
                    break;
                case WebEvent.TYPE_BLUR_ANNOTATION:
                    if (sourceWebViewViewModel.getFocusedAnnotation().getValue() != null) {
                        handleAnnotationBlur();
                    }
                    break;
                case WebEvent.TYPE_SELECTION_CREATED:
                    try {
                        JSONObject rawBoundingBox = event.getData().getJSONObject("boundingBox");
                        Rect boundingBox = new Rect(
                                rawBoundingBox.getInt("left"),
                                rawBoundingBox.getInt("top"),
                                rawBoundingBox.getInt("right"),
                                rawBoundingBox.getInt("bottom")
                        );
                        webView.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_DOWN, (float) boundingBox.left, (float) boundingBox.top, 0));
                        webView.dispatchTouchEvent(MotionEvent.obtain(SystemClock.uptimeMillis(), SystemClock.uptimeMillis(), MotionEvent.ACTION_UP, (float) boundingBox.left, (float) boundingBox.top, 0));
                    } catch (JSONException ex) {
                        Log.d("SourceWebView", "Unable to parse event data: " + event.getData(), ex);
                    }
                case WebEvent.TYPE_ANNOTATION_RENDERER_INITIALIZED:
                    if (!sourceWebViewViewModel.getHasFinishedInitializing().getValue()) {
                        sourceWebViewViewModel.setHasFinishedInitializing(true);
                    }
                    break;
            }
        });

        appWebViewViewModel.getReceivedWebEvents().observe(requireActivity(), (webEvents) -> {
            if (webEvents == null) {
                return;
            }

            webEvents.iterator().forEachRemaining((webEvent) -> {
                switch (webEvent.getType()) {
                    case WebEvent.TYPE_SET_VIEW_STATE:
                        try {
                            String state = webEvent.getData().getString("state");
                            this.handleSetBottomSheetState(state);
                        } catch (JSONException e) {
                            Log.d("SourceWebView", "Unable to handle SET_VIEW_STATE", e);
                        }
                        return;
                    case WebEvent.TYPE_EDIT_ANNOTATION_TAGS_RESULT:
                        try {
                            Annotation newAnnotation = Annotation.fromJson(webEvent.getData());
                            boolean updated = sourceWebViewViewModel.updateAnnotation(newAnnotation);
                            if (!updated) {
                                Log.d("SourceWebView", "Unable to update annotation: " + webEvent.getData());
                            }
                        } catch (JSONException e) {
                            Log.d("SourceWebView", "Unable to handle EDIT_ANNOTATION_TAGS_RESULT", e);
                        }
                        return;
                }
            });

            appWebViewViewModel.clearReceivedWebEvents();
        });

        appWebViewViewModel.getBottomSheetState().observe(requireActivity(), (bottomSheetState) -> {
            switch (bottomSheetState) {
                case BottomSheetBehavior.STATE_COLLAPSED:
                    appBarLayout.setExpanded(true);
                    break;
                case BottomSheetBehavior.STATE_EXPANDED:
                    appBarLayout.setExpanded(false);
                    break;
            }
        });

        sourceWebViewViewModel.getWebEvents().observe(requireActivity(), (webEvents) -> {
            if (webEvents == null) {
                return;
            }

            webEvents.iterator().forEachRemaining((webEvent) -> {
                webView.postWebEvent(webEvent);
            });

            sourceWebViewViewModel.clearWebEvents();
        });

        if (savedInstanceState != null) {
            webView.restoreState(savedInstanceState);
        } else {
            webView.loadUrl(paramInitialUrl);
        }
    }

    private void handleSetBottomSheetState(String state) {
        if (editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
            isEditingAnnotation = false;
        }
        switch (state) {
            case "COLLAPSED_ANNOTATION_TAGS":
                appWebViewViewModel.setBottomSheetState(
                        sourceWebViewViewModel.getFocusedAnnotation().getValue() != null ? BottomSheetBehavior.STATE_COLLAPSED : BottomSheetBehavior.STATE_HIDDEN
                );
                break;
            case "EDIT_ANNOTATION_TAGS":
                appWebViewViewModel.setBottomSheetState(
                        sourceWebViewViewModel.getFocusedAnnotation().getValue() != null ? BottomSheetBehavior.STATE_EXPANDED : BottomSheetBehavior.STATE_HIDDEN
                );
                break;
        }
    }

    private String getAnnotationRendererScript() {
        ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
        Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();

        try {
            JSONArray paramAnnotations = JsonArrayUtil.stringifyObjectArray(annotations.toArray(new Annotation[0]), Annotation::toJson);
            return sourceWebViewViewModel.getAnnotationRendererScript(
                    getActivity().getAssets(),
                    paramAnnotations,
                    focusedAnnotation != null ? focusedAnnotation.getId() : ""
            );
        } catch (JSONException ex) {
            Log.d("SourceWebView", "Unable to highlightAnnotationTargets", ex);
        }
        return null;
    }

    private void handleAnnotationCreated() {
        String script = sourceWebViewViewModel.getGetAnnotationScript(getActivity().getAssets());
        webView.evaluateJavascript(script, value -> {
            String creatorUsername = authenticationViewModel.getUsername().getValue();
            Annotation annotation = sourceWebViewViewModel.createAnnotation(value, creatorUsername);
            sourceWebViewViewModel.setFocusedAnnotation(annotation);
            appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_EXPANDED);
            Callback<Exception, AmazonS3URI> onArchiveUploaded = (e, amazonS3URI) -> {
                if (e != null) {
                    Log.d("SourceWebView", "onArchiveUploaded error", e);
                }
                Log.i("SourceWebView", "onArchiveUploaded: " + amazonS3URI.toString());
                TimeState timeState = new TimeState(
                        new String[]{amazonS3URI.toString()},
                        new String[]{DateUtil.toISO8601UTC(new Date())}
                );
                Optional<Annotation> update = sourceWebViewViewModel.getAnnotations().getValue()
                        .stream()
                        .filter((a) -> a.getId().equals(annotation.getId()))
                        .findFirst()
                        .map(annotationToUpdate -> {
                            Target[] target = annotationToUpdate.getTarget();
                            for (int i = 0; i < target.length; i++) {
                                if (target[i].getType().equals(Target.Type.SPECIFIC_TARGET)) {
                                    SpecificTarget specificTarget = (SpecificTarget) target[i];
                                    target[i] = new SpecificTarget(
                                            specificTarget.getId(),
                                            specificTarget.getSource(),
                                            specificTarget.getSelector(),
                                            new State[]{timeState}
                                    );
                                    break;
                                }
                            }

                            return new Annotation(
                                    annotationToUpdate.getBody(),
                                    target,
                                    annotationToUpdate.getMotivation(),
                                    annotationToUpdate.getCreated(),
                                    annotationToUpdate.getModified(),
                                    annotationToUpdate.getId()
                            );
                        });

                if (!update.isPresent()) {
                    Log.d("SourceWebView", "Expected annotation update, but found none.");
                    return;
                }

                try {
                    Log.i("SourceWebView", "Updated annotation: " + update.get().toJson());
                } catch (JSONException jsonException) {
                    Log.d("SourceWebView", "JSONException", jsonException);
                }

                sourceWebViewViewModel.updateAnnotation(update.get());
            };

            webView.saveWebArchive(
                    ArchiveRepository.getLocalDir(getContext()).getAbsolutePath(),
                    true,
                    (filePath) -> {
                        ArchiveRepository.upload(
                                getContext(),
                                new File(filePath),
                                authenticationViewModel.getIdentityId().getValue(),
                                onArchiveUploaded
                        );
                    }
            );
        });
    }

    private void handleAnnotationFocus(String annotationId, Rect annotationBoundingBox) {
        Optional<Annotation> annotation = sourceWebViewViewModel
                .getAnnotations()
                .getValue()
                .stream()
                .filter((a) -> a.getId() != null && a.getId().equals(annotationId))
                .findFirst();

        if (!annotation.isPresent()) {
            Log.d("SourceWebView", "handleAnnotationClicked unable to find annotationId");
        }
        Annotation unwrappedAnnotation = annotation.get();

        Annotation currentFocusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (currentFocusedAnnotation == null || !currentFocusedAnnotation.getId().equals(unwrappedAnnotation.getId())) {
            sourceWebViewViewModel.setFocusedAnnotation(unwrappedAnnotation);
        }

        appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_COLLAPSED);

        if (editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
        }

        try {
            String getAnnotationBoundingBoxScript = sourceWebViewViewModel.getGetAnnotationBoundingBoxScript(
                    getActivity().getAssets(),
                    annotation.get().toJson()
            );
            editAnnotationActionMode = webView.startEditAnnotationActionMode(
                    getAnnotationBoundingBoxScript,
                    annotationBoundingBox,
                    (_e, _d) -> {
                        handleAnnotationEdit(annotationId);
                    },
                    (_e, _d) -> {
                        handleAnnotationDelete(annotationId);
                    }
            );
        } catch (JSONException e) {
            Log.d("SourceWebView", "Unable to stringify annotation: " + annotation.get(), e);
        }
    }

    private void handleAnnotationCancelEdit() {
        this.handleRenderAnnotations(
                sourceWebViewViewModel.getAnnotations().getValue(),
                sourceWebViewViewModel.getFocusedAnnotation().getValue()
        );
        Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (focusedAnnotation != null) {
            webView.postWebEvent(new WebEvent(
                    WebEvent.TYPE_BLUR_ANNOTATION,
                    UUID.randomUUID().toString(),
                    new JSONObject()
            ));
            sourceWebViewViewModel.setFocusedAnnotation(null);
        }
        if (editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
            isEditingAnnotation = false;
        }
    }

    private void handleAnnotationCommitEdit() {
        String script = sourceWebViewViewModel.getGetAnnotationScript(getActivity().getAssets());
        Annotation annotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (annotation == null) {
            Log.d("SourceWebView", "Expected focusedAnnotation, but found null.");
            return;
        }

        webView.evaluateJavascript(script, value -> {
            try {
                Annotation parsedAnnotation = Annotation.fromJson(new JSONObject(value));

                ArrayList<Target> updatedTarget = new ArrayList<>();
                ArrayList<PatchAnnotationOperationInput> patchAnnotationOperationInputs = new ArrayList<>();
                Arrays.stream(parsedAnnotation.getTarget()).forEach((target) -> {
                    Optional<Target> existingTarget = Arrays
                            .stream(annotation.getTarget())
                            .filter((t) -> t.getType().equals(target.getType()))
                            .findFirst();
                    if (!existingTarget.isPresent()) {
                        updatedTarget.add(target);
                        patchAnnotationOperationInputs.add(target.toPatchAnnotationOperationInputAdd());
                    }

                    if (target.getType() == Target.Type.SPECIFIC_TARGET) {
                        SpecificTarget existingSpecificTarget = (SpecificTarget) target;
                        SpecificTarget updatedSpecificTarget = new SpecificTarget(
                                ((SpecificTarget) existingTarget.get()).getId(),
                                existingSpecificTarget.getSource(),
                                existingSpecificTarget.getSelector(),
                                existingSpecificTarget.getState()
                        );
                        updatedTarget.add(updatedSpecificTarget);
                        patchAnnotationOperationInputs.add(updatedSpecificTarget.toPatchAnnotationOperationInputSet());
                    } else if (target.getType() == Target.Type.EXTERNAL_TARGET) {
                        ExternalTarget existingExternalTarget = (ExternalTarget) target;
                        ExternalTarget updatedExternalTarget = new ExternalTarget(
                                ((ExternalTarget) existingTarget.get()).getId(),
                                existingExternalTarget.getFormat(),
                                existingExternalTarget.getLanguage(),
                                existingExternalTarget.getProcessingLanguage(),
                                existingExternalTarget.getTextDirection(),
                                existingExternalTarget.getAccessibility(),
                                existingExternalTarget.getRights(),
                                existingExternalTarget.getResourceType()
                        );
                        updatedTarget.add(updatedExternalTarget);
                        patchAnnotationOperationInputs.add(updatedExternalTarget.toPatchAnnotationOperationInputSet());
                    } else if (target.getType() == Target.Type.TEXTUAL_TARGET) {
                        TextualTarget existingTextualTarget = (TextualTarget) target;
                        TextualTarget updatedTextualTarget = new TextualTarget(
                                ((TextualTarget) existingTarget.get()).getId(),
                                existingTextualTarget.getFormat(),
                                existingTextualTarget.getLanguage(),
                                existingTextualTarget.getProcessingLanguage(),
                                existingTextualTarget.getTextDirection(),
                                existingTextualTarget.getAccessibility(),
                                existingTextualTarget.getRights(),
                                existingTextualTarget.getValue()
                        );
                        updatedTarget.add(updatedTextualTarget);
                        patchAnnotationOperationInputs.add(updatedTextualTarget.toPatchAnnotationOperationInputSet());
                    }
                });
                Annotation updatedAnnotation = new Annotation(
                        annotation.getBody(),
                        updatedTarget.toArray(new Target[0]),
                        annotation.getMotivation(),
                        annotation.getCreated(),
                        annotation.getModified(),
                        annotation.getId()
                );
                String username = authenticationViewModel.getUsername().getValue();
                if (username == null) {
                    Log.d("SourceWebView", "Expected username, but found none");
                    return;
                }
                PatchAnnotationInput input = PatchAnnotationInput.builder()
                        .creatorUsername(username)
                        .id(updatedAnnotation.getId())
                        .operations(patchAnnotationOperationInputs).build();
                AnnotationRepository.patchAnnotationMutation(
                        getContext(),
                        input,
                        (e, data) -> {
                            if (e != null) {
                                Log.d("SourceWebView", "Unable to handleAnnotationCommit", e);
                                return;
                            }
                            try {
                                JSONObject setCacheAnnotationData = new JSONObject();
                                setCacheAnnotationData.put("annotation", updatedAnnotation.toJson());
                                getActivity().runOnUiThread(() -> appWebViewViewModel.dispatchWebEvent(
                                        new WebEvent(
                                                WebEvent.TYPE_SET_CACHE_ANNOTATION,
                                                UUID.randomUUID().toString(),
                                                setCacheAnnotationData
                                        )
                                ));
                            } catch (JSONException ex) {
                                Log.d("SourceWebView", "Unable to serialize annotation: " + annotation, ex);
                            }
                        }
                );
                boolean updated = sourceWebViewViewModel.updateAnnotation(updatedAnnotation);
                if (!updated) {
                    Log.d("SourceWebView", "Failed to update viewmodel for annotation");
                }

            } catch (JSONException e) {
                Log.d("SourceWebView", "Unable to parse annotation: " + value, e);
            } finally {
                if (editAnnotationActionMode != null) {
                    webView.finishEditAnnotationActionMode(editAnnotationActionMode);
                    editAnnotationActionMode = null;
                    isEditingAnnotation = false;
                }

                Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
                if (focusedAnnotation != null) {
                    webView.postWebEvent(new WebEvent(
                            WebEvent.TYPE_BLUR_ANNOTATION,
                            UUID.randomUUID().toString(),
                            new JSONObject()
                    ));
                    sourceWebViewViewModel.setFocusedAnnotation(null);
                }
            }
        });

    }

    public void handleViewTargetForAnnotation(Annotation annotation, String targetId) {

        Optional<Target> targetOption = Arrays.stream(annotation.getTarget()).filter((t) -> {
            if (t.getType().equals(Target.Type.EXTERNAL_TARGET)) {
                return ((ExternalTarget) t).getId().equals(targetId);
            } else if (t.getType().equals(Target.Type.SPECIFIC_TARGET)) {
                return ((SpecificTarget) t).getId().equals(targetId);
            } else if (t.getType().equals(Target.Type.TEXTUAL_TARGET)) {
                return ((TextualTarget) t).getId().equals(targetId);
            }
            return false;
        }).findFirst();

        if (!targetOption.isPresent()) {
            Log.d("SourceWebView", "Could not find target for annotation: " + annotation + ", target: " + targetId);
            return;
        }
        Target target = targetOption.get();

        String sourceUrl = null;
        String currentWebViewUrl = webView.getUrl();
        if (target.getType() == Target.Type.SPECIFIC_TARGET) {
            Target source = ((SpecificTarget) target).getSource();
            if (source.getType() == Target.Type.EXTERNAL_TARGET) {
                sourceUrl = ((ExternalTarget) source).getId();
            }
        } else if (target.getType() == Target.Type.EXTERNAL_TARGET) {
            sourceUrl = ((ExternalTarget) target).getId();
        }
        if (sourceUrl == null) {
            Log.d("SourceWebView", "handleViewTargetForAnnotation: Unable to parse sourceUrl from Target");
            return;
        }

        if (currentWebViewUrl == null || !currentWebViewUrl.equals(sourceUrl)) {
            sourceWebViewViewModel.setHasFinishedInitializing(false);
        }

        ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
        if (annotations
                .stream()
                .noneMatch(committedAnnotation -> committedAnnotation.getId().equals(annotation.getId()))) {
            sourceWebViewViewModel.addAnnotation(annotation);
        }

        sourceWebViewViewModel.setFocusedAnnotation(annotation);

        if (currentWebViewUrl == null || !currentWebViewUrl.equals(sourceUrl)) {
            try {
                sourceWebViewViewModel.setDomainMetadata(new URL(sourceUrl), null);
            } catch (MalformedURLException e) {
                Log.d("SourceWebView", "Unable to parse URL", e);
            }
            webView.loadUrl(sourceUrl);
            shouldClearHistoryOnPageFinished = true;
        }
    }

    private void dispatchFocusAnnotationWebEvent(Annotation focusedAnnotation) {
        if (focusedAnnotation != null && this.isWebviewInitialized()) {
            try {
                JSONObject focusAnnotationData = new JSONObject();
                focusAnnotationData.put("annotationId", focusedAnnotation.getId());
                webView.postWebEvent(new WebEvent(
                        WebEvent.TYPE_FOCUS_ANNOTATION,
                        UUID.randomUUID().toString(),
                        focusAnnotationData
                ));
            } catch (JSONException ex) {
                Log.d("SourceWebView", "Unable to dispatchFocusAnnotationWebEvent", ex);
            }
        }
    }

    private void handleRenderAnnotations(ArrayList<Annotation> annotations, Annotation focusedAnnotation) {
        if (!this.isWebviewInitialized()) {
            Log.d("SourceWebView", "handleRenderAnnotations: expected webview to be initialized, nooping.");
            return;
        }

        try {
            JSONObject renderAnnotationsData = new JSONObject();
            renderAnnotationsData.put("annotations", JsonArrayUtil.stringifyObjectArray(annotations.toArray(new Annotation[0]), Annotation::toJson));
            if (focusedAnnotation != null) {
                renderAnnotationsData.put("focusedAnnotationId", focusedAnnotation.getId());
            }
            webView.postWebEvent(new WebEvent(
                    WebEvent.TYPE_RENDER_ANNOTATIONS,
                    UUID.randomUUID().toString(),
                    renderAnnotationsData
            ));
        } catch (JSONException ex) {
            Log.d("SourceWebView", "handleRenderAnnotations: Unable to dispatch RENDER_ANNOTATIONS.", ex);
        }
    }

    public void handleAnnotationBlur() {
        appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);
        sourceWebViewViewModel.setFocusedAnnotation(null);
        if (editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
            isEditingAnnotation = false;
        }
    }

    private void handleAnnotationEdit(String annotationId) {
        if (this.editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
            isEditingAnnotation = false;
        }
        isEditingAnnotation = true;
        try {
            JSONObject editAnnotationData = new JSONObject();
            editAnnotationData.put("annotationId", annotationId);
            webView.postWebEvent(new WebEvent(
                    WebEvent.TYPE_EDIT_ANNOTATION,
                    UUID.randomUUID().toString(),
                    editAnnotationData
            ));
        } catch (JSONException ex) {
            Log.d("SourceWebView", "Unable to dispatch editAnnotation event", ex);
        }
    }

    private void handleAnnotationDelete(String annotationId) {
        Annotation annotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (annotation == null) {
            Log.d("SourceWebView", "handleAnnotationDelete expected focusedAnnotation, but found null.");
            return;
        }

        sourceWebViewViewModel.setFocusedAnnotation(null);

        if (this.editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
            isEditingAnnotation = false;
        }

        boolean updated = sourceWebViewViewModel.removeAnnotation(annotationId);
        if (!updated) {
            Log.d("SourceWebView", "handleAnnotationDelete: Unable to find annotation for id " + annotationId);
            return;
        }

        AnnotationRepository.deleteAnnotationMutation(
                getContext(),
                DeleteAnnotationInput.builder()
                        .creatorUsername(authenticationViewModel.getUsername().getValue())
                        .id(annotationId)
                        .build(),
                (e, _data) -> {
                    if (e != null) {
                        Log.d("SourceWebView", "Unable to handleAnnotationDelete", e);
                        return;
                    }
                    try {
                        JSONObject deleteCacheAnnotationData = new JSONObject();
                        deleteCacheAnnotationData.put("annotation", annotation.toJson());
                        getActivity().runOnUiThread(() -> appWebViewViewModel.dispatchWebEvent(
                                new WebEvent(
                                        WebEvent.TYPE_DELETE_CACHE_ANNOTATION,
                                        UUID.randomUUID().toString(),
                                        deleteCacheAnnotationData
                                )
                        ));
                    } catch (JSONException ex) {
                        Log.d("SourceWebView", "Unable to serialize annotation: " + annotation, ex);
                    }
                }
        );
    }

    public void setOnToolbarPrimaryActionCallback(Callback<Void, Annotation[]> onToolbarPrimaryActionCallback) {
        this.onToolbarPrimaryActionCallback = onToolbarPrimaryActionCallback;
    }

    private void handleToolbarPrimaryAction() {
        ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
        ArrayList<String> createdAnnotationIds = sourceWebViewViewModel.getCreatedAnnotationIds();
        Annotation[] createdAnnotations = new Annotation[0];
        if (createdAnnotationIds != null && annotations != null && createdAnnotationIds.size() > 0) {
            createdAnnotations = annotations.stream().filter((annotation) -> createdAnnotationIds.contains(annotation.getId())).toArray(Annotation[]::new);

            ShareTargetHandlerRepository.createAnnotations(getContext(), createdAnnotations, (e, data) -> {
                if (e != null) {
                    Log.d("SourceWebView", "handleDone", e);
                }
            });
        }
        if (onToolbarPrimaryActionCallback != null) {
            onToolbarPrimaryActionCallback.invoke(
                    null,
                    createdAnnotations
            );
        } else {
            DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            if (domainMetadata != null) {
                NotificationRepository.sourceCreatedNotification(
                        getContext(),
                        authenticationViewModel.getUsername().getValue(),
                        domainMetadata
                );
            }
            Activity activity = getActivity();
            if (activity != null) {
                activity.setResult(Activity.RESULT_CANCELED);
                activity.finish();
            }
        }
    }

    public io.literal.ui.view.SourceWebView getWebView() { return webView; }

    /**
     * Message channel is established and the desired page is loaded
     **/
    private boolean isWebviewInitialized() {
        return sourceWebViewViewModel != null && sourceWebViewViewModel.getHasFinishedInitializing() != null && sourceWebViewViewModel.getHasFinishedInitializing().getValue();
    }

    @Override
    public void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        if (this.webView != null) {
            webView.saveState(outState);
        }
    }

    @Override
    public void onCreateOptionsMenu(@NonNull Menu menu, @NonNull MenuInflater inflater) {
        inflater.inflate(R.menu.source_webview_toolbar, menu);

        menu.getItem(0).setIcon(paramToolbarPrimaryActionResourceId);
        super.onCreateOptionsMenu(menu, inflater);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()) {
            case R.id.primary:
                this.handleToolbarPrimaryAction();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}