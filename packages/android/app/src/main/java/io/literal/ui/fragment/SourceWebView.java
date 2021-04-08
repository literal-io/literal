package io.literal.ui.fragment;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.graphics.drawable.BitmapDrawable;
import android.net.Uri;
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
import android.webkit.WebView;
import android.webkit.WebViewClient;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.services.kms.model.EnableKeyRotationRequest;
import com.google.android.gms.auth.api.signin.internal.Storage;
import com.google.android.material.appbar.AppBarLayout;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Text;

import java.lang.reflect.Array;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Optional;
import java.util.UUID;
import java.util.function.Supplier;
import java.util.stream.Collectors;

import io.literal.R;
import io.literal.lib.Callback;
import io.literal.lib.Callback2;
import io.literal.lib.DateUtil;
import io.literal.lib.DomainMetadata;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.WebEvent;
import io.literal.model.Annotation;
import io.literal.model.Body;
import io.literal.model.ExternalTarget;
import io.literal.model.SpecificTarget;
import io.literal.model.State;
import io.literal.model.Target;
import io.literal.model.TextualBody;
import io.literal.model.TextualTarget;
import io.literal.model.TimeState;
import io.literal.repository.AnnotationRepository;
import io.literal.repository.ArchiveRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.StorageRepository;
import io.literal.service.AnnotationService;
import io.literal.ui.MainApplication;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;
import type.DeleteAnnotationInput;
import type.PatchAnnotationInput;
import type.PatchAnnotationOperationInput;

public class SourceWebView extends Fragment {

    private static final String PARAM_INITIAL_URL = "PARAM_INITIAL_URL";
    private static final String PARAM_BOTTOM_SHEET_APP_WEB_VIEW_VIEW_MODEL_KEY = "PARAM_BOTTOM_SHEET_APP_WEB_VIEW_VIEW_MODEL_KEY";
    private static final String PARAM_TOOLBAR_PRIMARY_ACTION_ICON_RESOURCE_ID = "PARAM_PRIMARY_TOOLBAR_ACTION_ICON_RESOURCE_ID";
    private static final String PARAM_PRIMARY_APP_WEB_VIEW_VIEW_MODEL_KEY = "PARAM_PRIMARY_APP_WEB_VIEW_VIEW_MODEL_KEY";

    private String paramInitialUrl;
    private String paramPrimaryAppWebViewViewModelKey;
    private String paramBottomSheetAppWebViewViewModelKey;
    private int paramToolbarPrimaryActionResourceId;

    private io.literal.ui.view.SourceWebView webView;
    private Toolbar toolbar;
    private AppBarLayout appBarLayout;

    /** Triggered when the primary action in the toolbar tapped. **/
    private Callback2<Annotation[], DomainMetadata> onToolbarPrimaryActionCallback;
    /** Triggered when ShareTargetHandler should be opened to URL **/
    private Callback<Void, URL> onCreateAnnotationFromSource;

    private ActionMode editAnnotationActionMode;
    /**
     * Show a different CAB if text is selected while editing annotation
     **/
    private boolean isEditingAnnotation;

    private boolean shouldClearHistoryOnPageFinished;

    private SourceWebViewViewModel sourceWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private AppWebViewViewModel bottomSheetAppWebViewViewModel;
    private AppWebViewViewModel primaryAppWebViewViewModel;

    public static SourceWebView newInstance(@NotNull String initialUrl, String bottomSheetAppWebViewViewModelKey, String primaryAppWebViewViewModelKey, @NotNull int toolbarPrimaryActionResourceId) {
        SourceWebView fragment = new SourceWebView();
        Bundle args = new Bundle();
        args.putString(PARAM_INITIAL_URL, initialUrl);
        args.putString(PARAM_BOTTOM_SHEET_APP_WEB_VIEW_VIEW_MODEL_KEY, bottomSheetAppWebViewViewModelKey);
        args.putString(PARAM_PRIMARY_APP_WEB_VIEW_VIEW_MODEL_KEY, primaryAppWebViewViewModelKey);
        args.putInt(PARAM_TOOLBAR_PRIMARY_ACTION_ICON_RESOURCE_ID, toolbarPrimaryActionResourceId);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            paramInitialUrl = getArguments().getString(PARAM_INITIAL_URL);
            paramBottomSheetAppWebViewViewModelKey = getArguments().getString(PARAM_BOTTOM_SHEET_APP_WEB_VIEW_VIEW_MODEL_KEY);
            paramPrimaryAppWebViewViewModelKey = getArguments().getString(PARAM_PRIMARY_APP_WEB_VIEW_VIEW_MODEL_KEY);
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
        bottomSheetAppWebViewViewModel =
                paramBottomSheetAppWebViewViewModelKey != null
                        ? new ViewModelProvider(requireActivity()).get(paramBottomSheetAppWebViewViewModelKey, AppWebViewViewModel.class)
                        : new ViewModelProvider(requireActivity()).get(AppWebViewViewModel.class);
        if (paramPrimaryAppWebViewViewModelKey != null) {
            primaryAppWebViewViewModel = new ViewModelProvider(requireActivity()).get(paramPrimaryAppWebViewViewModelKey, AppWebViewViewModel.class);
        }

        toolbar = view.findViewById(R.id.toolbar);
        ((AppCompatActivity) getActivity()).setSupportActionBar(toolbar);

        appBarLayout = view.findViewById(R.id.app_bar_layout);

        webView = view.findViewById(R.id.source_web_view);
        webView.setOnReceivedIcon((e, webView, icon) -> {
            if (e != null) {
                ErrorRepository.captureException(e);
            }

            DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
            sourceWebViewViewModel.setDomainMetadata(DomainMetadata.updateFavicon(domainMetadata, icon));
        });
        webView.setOnGetWebMessageChannelInitializerScript((_e, _data) -> {
            sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(true);
            return getAnnotationRendererScript();
        });

        webView.setExternalWebViewClient(new WebViewClient() {
            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                try {
                    URL newUrl = new URL(url);
                    DomainMetadata domainMetadata = Optional.ofNullable(sourceWebViewViewModel.getDomainMetadata().getValue())
                            .map(inst -> DomainMetadata.updateUrl(inst, newUrl))
                            .orElse(new DomainMetadata(newUrl, null));
                    sourceWebViewViewModel.setDomainMetadata(domainMetadata);
                } catch (MalformedURLException ex) {
                    ErrorRepository.captureException(ex);
                }
                sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(false);
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                if (!sourceWebViewViewModel.getHasInjectedAnnotationRendererScript().getValue()) {
                    sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(true);
                    webView.evaluateJavascript(getAnnotationRendererScript(), _v -> { /** noop **/});
                }

                if (shouldClearHistoryOnPageFinished) {
                    view.clearHistory();
                }
            }
        });

        webView.setOnAnnotationCreated((e, actionMode) -> {
            if (e != null) {
                ErrorRepository.captureException(e);
                return;
            }
            this.handleAnnotationCreated(actionMode);

        });
        webView.setOnAnnotationCancelEdit((e, actionMode) -> {
            this.handleAnnotationCancelEdit(actionMode);
        });
        webView.setOnAnnotationCommitEdit((e, actionMode) -> {
            this.handleAnnotationCommitEdit(actionMode);
        });
        webView.setOnGetTextSelectionMenu((e, data) -> isEditingAnnotation
                ? R.menu.source_webview_commit_edit_annotation_menu
                : R.menu.source_webview_create_annotation_menu);

        sourceWebViewViewModel.getAnnotations().observe(getActivity(), (annotations) -> {
            if (this.webView == null) {
                ErrorRepository.captureException(new Exception("Expected webView, but found none."));
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
                toolbar.setTitle(domainMetadata.getCannonicalUrl().getHost());
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
                        ErrorRepository.captureException(ex, event.getData().toString());
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
                        ErrorRepository.captureException(ex, event.getData().toString());
                    }
                    break;
                case WebEvent.TYPE_ANNOTATION_RENDERER_INITIALIZED:
                    if (!sourceWebViewViewModel.getHasFinishedInitializing().getValue()) {
                        sourceWebViewViewModel.setHasFinishedInitializing(true);
                    }
                    break;
                case WebEvent.TYPE_SELECTION_CHANGE:
                    this.handleSelectionChange(event.getData().optBoolean("isCollapsed", true));
                    break;
            }
        });

        bottomSheetAppWebViewViewModel.getReceivedWebEvents().observe(requireActivity(), (webEvents) -> {
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
                            ErrorRepository.captureException(e);
                        }
                        return;
                    case WebEvent.TYPE_EDIT_ANNOTATION_TAGS_RESULT:
                        try {
                            Annotation newAnnotation = Annotation.fromJson(webEvent.getData());
                            this.handleAnnotationTextualBodyChange(newAnnotation);
                        } catch (JSONException e) {
                            ErrorRepository.captureException(e);
                        }
                        return;
                }
            });

            bottomSheetAppWebViewViewModel.clearReceivedWebEvents();
        });

        bottomSheetAppWebViewViewModel.getBottomSheetState().observe(requireActivity(), (bottomSheetState) -> {
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
                bottomSheetAppWebViewViewModel.setBottomSheetState(
                        sourceWebViewViewModel.getFocusedAnnotation().getValue() != null ? BottomSheetBehavior.STATE_COLLAPSED : BottomSheetBehavior.STATE_HIDDEN
                );
                break;
            case "EDIT_ANNOTATION_TAGS":
                bottomSheetAppWebViewViewModel.setBottomSheetState(
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
            ErrorRepository.captureException(ex);
        }
        return null;
    }

    private void handleAnnotationCreated(ActionMode mode) {
        try {
            /**
             * If we're trying to create a new annotation from an archived source, open the cannonical
             * source instead and prompt the annotation to be created there. There's definitely a better
             * UX for this.
             */
            if (StorageRepository.isStorageUrl(getContext(), new URL(webView.getUrl()))
                    && this.sourceWebViewViewModel.getDomainMetadata().getValue().getCannonicalUrl() != null
                    && this.onCreateAnnotationFromSource != null) {
                this.onCreateAnnotationFromSource.invoke(null, this.sourceWebViewViewModel.getDomainMetadata().getValue().getCannonicalUrl());
                return;
            }
        } catch (MalformedURLException e) {
            ErrorRepository.captureException(e);
        }

        String script = sourceWebViewViewModel.getGetAnnotationScript(getActivity().getAssets());
        webView.evaluateJavascript(script, value -> {
            mode.finish();
            webView.saveWebArchive(
                    ArchiveRepository.getLocalDir(getContext()).getAbsolutePath() + "/" + UUID.randomUUID().toString() + ".mhtml",
                    false,
                    (filePath) -> {
                        String creatorUsername = authenticationViewModel.getUsername().getValue();
                        Annotation annotation = sourceWebViewViewModel.createAnnotation(value, creatorUsername, false);

                        TimeState timeState = new TimeState(
                                new String[]{"file://" + filePath},
                                new String[]{DateUtil.toISO8601UTC(new Date())}
                        );

                        Target[] target = annotation.getTarget();
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

                        sourceWebViewViewModel.createAnnotation(annotation);
                        sourceWebViewViewModel.setFocusedAnnotation(annotation);
                        bottomSheetAppWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_EXPANDED);
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
            ErrorRepository.captureException(new Exception("handleAnnotationClicked unable to find annotationId"));
            return;
        }
        Annotation unwrappedAnnotation = annotation.get();

        Annotation currentFocusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (currentFocusedAnnotation == null || !currentFocusedAnnotation.getId().equals(unwrappedAnnotation.getId())) {
            sourceWebViewViewModel.setFocusedAnnotation(unwrappedAnnotation);
        }

        bottomSheetAppWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_COLLAPSED);

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
            ErrorRepository.captureException(e);
        }
    }

    private void handleAnnotationCancelEdit(ActionMode mode) {
        if (mode != null) {
            mode.finish();
        }
        this.handleRenderAnnotations(
                sourceWebViewViewModel.getAnnotations().getValue(),
                sourceWebViewViewModel.getFocusedAnnotation().getValue()
        );
        Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (focusedAnnotation != null) {
            try {
                JSONObject blurAnnotationData = new JSONObject();
                blurAnnotationData.put("annotationId", focusedAnnotation.getId());
                webView.postWebEvent(new WebEvent(
                        WebEvent.TYPE_BLUR_ANNOTATION,
                        UUID.randomUUID().toString(),
                        blurAnnotationData
                ));
            } catch (JSONException ex) {
                ErrorRepository.captureException(ex);
            }
            sourceWebViewViewModel.setFocusedAnnotation(null);
        }
        if (editAnnotationActionMode != null) {
            webView.finishEditAnnotationActionMode(editAnnotationActionMode);
            editAnnotationActionMode = null;
        }
        isEditingAnnotation = false;
    }

    private void handleAnnotationCommitEdit(ActionMode mode) {
        String script = sourceWebViewViewModel.getGetAnnotationScript(getActivity().getAssets());
        Annotation annotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (annotation == null) {
            ErrorRepository.captureException(new Exception("Expected focusedAnnotation, but found null."));
            return;
        }

        webView.evaluateJavascript(script, value -> {
            mode.finish();
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
                    ErrorRepository.captureException(new Exception("Expected username, but found none"));
                    return;
                }

                if (!sourceWebViewViewModel.getCreatedAnnotationIds().contains(updatedAnnotation.getId())) {
                    PatchAnnotationInput input = PatchAnnotationInput.builder()
                            .creatorUsername(username)
                            .id(updatedAnnotation.getId())
                            .operations(patchAnnotationOperationInputs).build();
                    AnnotationRepository.patchAnnotationMutation(
                            getContext(),
                            input,
                            (e, data) -> {
                                if (e != null) {
                                    ErrorRepository.captureException(e);
                                    return;
                                }
                            }
                    );
                }
                boolean updated = sourceWebViewViewModel.updateAnnotation(updatedAnnotation);
                if (!updated) {
                    ErrorRepository.captureException(new Exception("Failed to update viewmodel for annotation"));
                }

                if (primaryAppWebViewViewModel != null) {
                    try {
                        JSONObject setCacheAnnotationData = new JSONObject();
                        setCacheAnnotationData.put("annotation", updatedAnnotation.toJson());
                        getActivity().runOnUiThread(() -> primaryAppWebViewViewModel.dispatchWebEvent(
                                new WebEvent(
                                        WebEvent.TYPE_SET_CACHE_ANNOTATION,
                                        UUID.randomUUID().toString(),
                                        setCacheAnnotationData
                                )
                        ));
                    } catch (JSONException ex) {
                        ErrorRepository.captureException(ex);
                    }
                }
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            } finally {
                if (editAnnotationActionMode != null) {
                    webView.finishEditAnnotationActionMode(editAnnotationActionMode);
                    editAnnotationActionMode = null;
                }
                isEditingAnnotation = false;

                Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
                if (focusedAnnotation != null) {
                    try {
                        JSONObject blurAnnotationData = new JSONObject();
                        blurAnnotationData.put("annotationId", focusedAnnotation.getId());
                        webView.postWebEvent(new WebEvent(
                                WebEvent.TYPE_BLUR_ANNOTATION,
                                UUID.randomUUID().toString(),
                                blurAnnotationData
                        ));
                    } catch (JSONException ex) {
                        ErrorRepository.captureException(ex);
                    }
                    sourceWebViewViewModel.setFocusedAnnotation(null);
                }
            }
        });
    }

    private void handleAnnotationTextualBodyChange(Annotation newAnnotation) {
        if (!sourceWebViewViewModel.getCreatedAnnotationIds().contains(newAnnotation.getId())) {
            Annotation annotation = sourceWebViewViewModel.getAnnotations().getValue().stream()
                    .filter(a -> a.getId().equals(newAnnotation.getId()))
                    .findFirst()
                    .get();
            List<TextualBody> body = Arrays.stream(Optional.ofNullable(annotation.getBody()).orElse(new Body[0]))
                .filter(b -> b.getType().equals(Body.Type.TEXTUAL_BODY))
                .map(b -> ((TextualBody) b))
                .collect(Collectors.toList());
            List<TextualBody> newBody = Arrays.stream(Optional.ofNullable(newAnnotation.getBody()).orElse(new Body[0]))
                .filter(b -> b.getType().equals(Body.Type.TEXTUAL_BODY))
                .map(b -> ((TextualBody) b))
                .collect(Collectors.toList());

            HashSet<String> bodyIds = body.stream().map(TextualBody::getId).collect(Collectors.toCollection((Supplier<HashSet<String>>) HashSet::new));
            HashSet<String> newBodyIds = newBody.stream().map(TextualBody::getId).collect(Collectors.toCollection((Supplier<HashSet<String>>) HashSet::new));

            HashSet<String> removedBodyIds = (HashSet<String>) bodyIds.clone();
            removedBodyIds.removeAll(newBodyIds);
            HashSet<String> addedBodyIds = (HashSet<String>) newBodyIds.clone();
            addedBodyIds.removeAll(bodyIds);

            ArrayList<PatchAnnotationOperationInput> operations = new ArrayList<>();
            operations.addAll(
                removedBodyIds.stream()
                        .map(id -> body.stream().filter(b -> b.getId().equals(id)).findFirst().get().toPatchAnnotationOperationInputRemove())
                        .collect(Collectors.toList())
            );
            operations.addAll(
                addedBodyIds.stream()
                        .map(id -> newBody.stream().filter(b -> b.getId().equals(id)).findFirst().get().toPatchAnnotationOperationInputAdd())
                        .collect(Collectors.toList())
            );

            AnnotationRepository.patchAnnotationMutation(
                    getContext(),
                    PatchAnnotationInput.builder()
                            .creatorUsername(authenticationViewModel.getUsername().getValue())
                            .id(newAnnotation.getId())
                            .operations(operations)
                            .build(),
                    (e, data) -> {
                        if (e != null) {
                            ErrorRepository.captureException(e);
                            return;
                        }
                    }
            );

            if (primaryAppWebViewViewModel != null) {
                try {
                    JSONObject setCacheAnnotationData = new JSONObject();
                    setCacheAnnotationData.put("annotation", newAnnotation.toJson());
                    getActivity().runOnUiThread(() -> primaryAppWebViewViewModel.dispatchWebEvent(
                            new WebEvent(
                                    WebEvent.TYPE_SET_CACHE_ANNOTATION,
                                    UUID.randomUUID().toString(),
                                    setCacheAnnotationData
                            )
                    ));
                } catch (JSONException ex) {
                    ErrorRepository.captureException(ex);
                }
            }
        }

        boolean updated = sourceWebViewViewModel.updateAnnotation(newAnnotation);
        if (!updated) {
            ErrorRepository.captureException(new Exception("Failed to update viewmodel for annotation"));
        }
    }

    private void handleSelectionChange(boolean isCollapsed) {
        if (isEditingAnnotation && isCollapsed) {
            this.handleAnnotationCancelEdit(null);
        }
    }

    public void handleViewTargetForAnnotation(Annotation annotation, String targetId, Callback<Exception, Void> onComplete) {

        Callback2<String, DomainMetadata> onTargetUrl = (e1, targetUrl, initialDomainMetadata) -> {
            if (e1 != null) {
                onComplete.invoke(e1, null);
                return;
            }

            String currentWebViewUrl = webView.getUrl();
            if (currentWebViewUrl == null || !currentWebViewUrl.equals(targetUrl)) {
                sourceWebViewViewModel.setHasFinishedInitializing(false);
                sourceWebViewViewModel.setHasInjectedAnnotationRendererScript(false);
                sourceWebViewViewModel.setAnnotations(new ArrayList<>());
                sourceWebViewViewModel.setCreatedAnnotationIds(new ArrayList<>());
            }

            ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
            if (annotations
                    .stream()
                    .noneMatch(committedAnnotation -> committedAnnotation.getId().equals(annotation.getId()))) {
                sourceWebViewViewModel.addAnnotation(annotation);
            }

            sourceWebViewViewModel.setFocusedAnnotation(annotation);

            if (currentWebViewUrl == null || !currentWebViewUrl.equals(targetUrl)) {
                sourceWebViewViewModel.setDomainMetadata(initialDomainMetadata);
                webView.loadUrl(targetUrl);
                shouldClearHistoryOnPageFinished = true;
            }
            onComplete.invoke(null, null);
        };

        Arrays.stream(annotation.getTarget())
                .filter((t) -> {
                    if (t.getType().equals(Target.Type.EXTERNAL_TARGET)) {
                        return ((ExternalTarget) t).getId().equals(targetId);
                    } else if (t.getType().equals(Target.Type.SPECIFIC_TARGET)) {
                        return ((SpecificTarget) t).getId().equals(targetId);
                    } else if (t.getType().equals(Target.Type.TEXTUAL_TARGET)) {
                        return ((TextualTarget) t).getId().equals(targetId);
                    }
                    return false;
                })
                .findFirst()
                .ifPresent((t) -> {
                    if (t.getType() == Target.Type.SPECIFIC_TARGET) {

                        String externalTargetUri = null;
                        Target source = ((SpecificTarget) t).getSource();
                        if (source.getType() == Target.Type.EXTERNAL_TARGET) {
                            externalTargetUri = ((ExternalTarget) source).getId();
                        }

                        Optional<String> cachedURL = Arrays.stream(Optional.ofNullable(((SpecificTarget) t).getState()).orElse(new State[0]))
                                .filter((s) -> s.getType().equals(State.Type.TIME_STATE))
                                .findFirst()
                                .flatMap((s) -> Arrays.stream(((TimeState) s).getCached()).filter(cachedUrl -> Uri.parse(cachedUrl).getScheme().equals("https")).findFirst());

                        if (cachedURL.isPresent()) {
                            String finalExternalTargetUri = externalTargetUri;
                            authenticationViewModel.awaitInitialization(
                                    ((MainApplication) getActivity().getApplication()).getThreadPoolExecutor(),
                                    (e, aVoid) -> getActivity().runOnUiThread(() -> {
                                        try {
                                            onTargetUrl.invoke(
                                                    null,
                                                    cachedURL.get(),
                                                    finalExternalTargetUri != null
                                                            ? new DomainMetadata(
                                                            new URL(cachedURL.get()),
                                                            new URL(finalExternalTargetUri),
                                                            null)
                                                            : null
                                            );
                                        } catch (MalformedURLException _e) {
                                            onTargetUrl.invoke(null, cachedURL.get(), null);
                                        }
                                    })
                            );
                            return;
                        }

                        if (externalTargetUri != null) {
                            try {
                                onTargetUrl.invoke(null, externalTargetUri, new DomainMetadata(new URL(externalTargetUri), null));
                            } catch (MalformedURLException e) {
                                onTargetUrl.invoke(e, null, null);
                            }
                            return;
                        }
                    }

                    if (t.getType() == Target.Type.EXTERNAL_TARGET) {
                        String id = ((ExternalTarget) t).getId();
                        try {
                            onTargetUrl.invoke(null, id, new DomainMetadata(new URL(id), null));
                        } catch (MalformedURLException e) {
                            onTargetUrl.invoke(e, null, null);
                        }
                        return;
                    }

                    onTargetUrl.invoke(new Exception("Unable to derive targetURL from target: " + t), null, null);
                });
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
                ErrorRepository.captureException(ex);
            }
        }
    }

    private void handleRenderAnnotations(ArrayList<Annotation> annotations, Annotation focusedAnnotation) {
        if (!this.isWebviewInitialized()) {
            ErrorRepository.captureWarning(new Exception("handleRenderAnnotations: expected webview to be initialized, nooping."));
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
            ErrorRepository.captureException(ex);
        }
    }

    public void handleAnnotationBlur() {
        bottomSheetAppWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);
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
            ErrorRepository.captureException(ex);
        }
    }

    private void handleAnnotationDelete(String annotationId) {
        Annotation annotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
        if (annotation == null) {
            ErrorRepository.captureException(new Exception("handleAnnotationDelete expected focusedAnnotation, but found null."));
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
            ErrorRepository.captureException(new Exception("handleAnnotationDelete: Unable to find annotation for id " + annotationId));
            return;
        }

        if (!sourceWebViewViewModel.getCreatedAnnotationIds().contains(annotationId)) {
            AnnotationRepository.deleteAnnotationMutation(
                    getContext(),
                    DeleteAnnotationInput.builder()
                            .creatorUsername(authenticationViewModel.getUsername().getValue())
                            .id(annotationId)
                            .build(),
                    (e, _data) -> {
                        if (e != null) {
                            ErrorRepository.captureException(e);
                            return;
                        }
                        try {
                            JSONObject deleteCacheAnnotationData = new JSONObject();
                            deleteCacheAnnotationData.put("annotation", annotation.toJson());
                            getActivity().runOnUiThread(() -> bottomSheetAppWebViewViewModel.dispatchWebEvent(
                                    new WebEvent(
                                            WebEvent.TYPE_DELETE_CACHE_ANNOTATION,
                                            UUID.randomUUID().toString(),
                                            deleteCacheAnnotationData
                                    )
                            ));
                        } catch (JSONException ex) {
                            ErrorRepository.captureException(ex);
                        }
                    }
            );
        }
    }

    public void setOnToolbarPrimaryActionCallback(Callback2<Annotation[], DomainMetadata> onToolbarPrimaryActionCallback) {
        this.onToolbarPrimaryActionCallback = onToolbarPrimaryActionCallback;
    }

    private void handleToolbarPrimaryAction() {
        ArrayList<Annotation> annotations = sourceWebViewViewModel.getAnnotations().getValue();
        DomainMetadata domainMetadata = sourceWebViewViewModel.getDomainMetadata().getValue();
        ArrayList<String> createdAnnotationIds = sourceWebViewViewModel.getCreatedAnnotationIds();
        Annotation[] createdAnnotations = new Annotation[0];

        if (createdAnnotationIds != null && annotations != null && createdAnnotationIds.size() > 0) {
            try {
                createdAnnotations = annotations.stream().filter((annotation) -> createdAnnotationIds.contains(annotation.getId())).toArray(Annotation[]::new);
                Intent serviceIntent = new Intent(getActivity(), AnnotationService.class);
                serviceIntent.setAction(AnnotationService.ACTION_CREATE_ANNOTATIONS);
                serviceIntent.putExtra(
                        AnnotationService.EXTRA_ANNOTATIONS,
                        JsonArrayUtil.stringifyObjectArray(createdAnnotations, Annotation::toJson).toString()
                );

                if (onToolbarPrimaryActionCallback == null && domainMetadata != null) {
                    serviceIntent.putExtra(
                            AnnotationService.EXTRA_DOMAIN_METADATA,
                            domainMetadata.toJson(getContext()).toString()
                    );
                }
                getActivity().startService(serviceIntent);
            } catch (JSONException ex) {
                ErrorRepository.captureException(ex);
            }
        }

        if (onToolbarPrimaryActionCallback != null) {
            onToolbarPrimaryActionCallback.invoke(
                    null,
                    createdAnnotations,
                    domainMetadata
            );
        } else {
            Activity activity = getActivity();
            if (activity != null) {
                activity.setResult(Activity.RESULT_CANCELED);
                activity.finish();
            }
        }
    }

    public io.literal.ui.view.SourceWebView getWebView() {
        return webView;
    }

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

    public void setOnCreateAnnotationFromSource(Callback<Void, URL> onCreateAnnotationFromSource) {
        this.onCreateAnnotationFromSource = onCreateAnnotationFromSource;
    }
}