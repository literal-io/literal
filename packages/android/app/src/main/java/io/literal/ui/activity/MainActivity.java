package io.literal.ui.activity;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobileconnectors.cognitoauth.AuthClient;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.CompletableFuture;

import io.literal.R;
import io.literal.lib.Callback;
import io.literal.lib.Constants;
import io.literal.lib.JsonArrayUtil;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.SourceInitializationStatus;
import io.literal.model.SourceWebViewAnnotation;
import io.literal.model.User;
import io.literal.repository.AnalyticsRepository;
import io.literal.repository.ErrorRepository;
import io.literal.repository.ToastRepository;
import io.literal.service.AnnotationService;
import io.literal.service.SplashService;
import io.literal.ui.fragment.AppWebView;
import io.literal.ui.fragment.AppWebViewBottomSheetAnimator;
import io.literal.ui.fragment.SourceWebView;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;

public class MainActivity extends InstrumentedActivity {

    private static final String APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME = "MAIN_ACTIVITY_APP_WEB_VIEW_PRIMARY_FRAGMENT";
    private static final String APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME = "MAIN_ACTIVITY_APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT";
    private static final String SOURCE_WEB_VIEW_FRAGMENT_NAME = "MAIN_ACTIVITY_SOURCE_WEB_VIEW_FRAGMENT";
    private AppWebViewViewModel appWebViewModelPrimary;
    private AppWebViewViewModel appWebViewViewModelBottomSheet;
    private SourceWebViewViewModel sourceWebViewViewModelBottomSheet;
    private AuthenticationViewModel authenticationViewModel;
    private AppWebView appWebViewPrimaryFragment = null;

    private final BroadcastReceiver annotationCreatedBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String annotationJSON = intent.getStringExtra(AnnotationService.EXTRA_ANNOTATION);
            if (appWebViewPrimaryFragment != null && annotationJSON != null) {
                try {
                    Annotation annotation = Annotation.fromJson(new JSONObject(annotationJSON));
                    Annotation[] annotations = { annotation };
                    JSONObject addCacheAnnotationsData = new JSONObject();
                    addCacheAnnotationsData.put("annotations", JsonArrayUtil.stringifyObjectArray(annotations, Annotation::toJson).toString());
                    appWebViewPrimaryFragment.postWebEvent(new WebEvent(
                            WebEvent.TYPE_ADD_CACHE_ANNOTATIONS,
                            UUID.randomUUID().toString(),
                            addCacheAnnotationsData
                    ));
                } catch (JSONException ex) {
                    ErrorRepository.captureException(ex);
                }
            }
        }
    };
    private SourceWebView sourceWebViewBottomSheetFragment = null;
    private AppWebView appWebViewBottomSheetFragment = null;
    private Observer<SourceInitializationStatus> sourceInitializationStatusObserver;
    private final ActivityResultLauncher<Intent> createAnnotationFromSourceLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), MainActivity.this::handleCreateAnnotationFromSourceResult);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        this.initializeViewModel().whenComplete((user, error) -> runOnUiThread(() -> {
            if (savedInstanceState == null) {
                String initialUrl;
                String defaultUrl = user.getState().equals(UserState.SIGNED_IN) || user.getState().equals(UserState.GUEST)
                        ? WebRoutes.creatorsIdAnnotationCollectionId(user.getAppSyncIdentity(), Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT)
                        : WebRoutes.authenticate();
                Intent intent = getIntent();
                if (intent != null) {
                    Uri uri = intent.getData();
                    if (uri != null) {
                        initialUrl = uri.toString();
                    } else {
                        initialUrl = defaultUrl;
                    }
                } else {
                    initialUrl = defaultUrl;
                }
                this.commitFragments(null, initialUrl, user);
            } else {
                this.commitFragments(savedInstanceState, null, user);
            }
        }));
        LocalBroadcastManager.getInstance(getBaseContext()).registerReceiver(
                annotationCreatedBroadcastReceiver,
                new IntentFilter(AnnotationService.ACTION_BROADCAST_CREATED_ANNOTATIONS)
        );

        try {
            JSONObject properties = new JSONObject();
            properties.put("name", "MainActivity");
            AnalyticsRepository.logEvent(AnalyticsRepository.TYPE_ACTIVITY_START, properties);
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        LocalBroadcastManager.getInstance(getBaseContext()).unregisterReceiver(annotationCreatedBroadcastReceiver);
    }

    private CompletableFuture<User> initializeViewModel() {
        authenticationViewModel = new ViewModelProvider(this).get(AuthenticationViewModel.class);
        CompletableFuture<User> userFuture = authenticationViewModel.initialize(this);
        authenticationViewModel.getUser().observe(this, user -> {
            if (appWebViewBottomSheetFragment == null ||
                    appWebViewModelPrimary == null ||
                    !appWebViewModelPrimary.getHasFinishedInitializing().getValue()) {
                return;
            }

            String paramInitialUrl = appWebViewBottomSheetFragment.getArguments().getString(AppWebView.PARAM_INITIAL_URL);
            String newParamInitialUrl = WebRoutes.creatorsIdWebview(user.getEncodedIdentityId());
            if (paramInitialUrl == null || !paramInitialUrl.equals(newParamInitialUrl)) {
                appWebViewBottomSheetFragment = AppWebView.newInstance(newParamInitialUrl, APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME);
                getSupportFragmentManager()
                        .beginTransaction()
                        .setReorderingAllowed(true)
                        .replace(R.id.app_web_view_bottom_sheet_fragment_container, appWebViewBottomSheetFragment)
                        .commit();
            }
        });

        appWebViewModelPrimary = new ViewModelProvider(this).get(APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME, AppWebViewViewModel.class);
        appWebViewModelPrimary.getHasFinishedInitializing().observe(this, hasFinishedInitializing -> {
            ViewGroup splashView = findViewById(R.id.splash);
            if (hasFinishedInitializing && SplashService.isVisible(splashView)) {
                SplashService.hide(this, splashView);
            }
        });
        appWebViewModelPrimary.getReceivedWebEvents().observe(this,
                (webEvents) -> {
                    if (webEvents == null) {
                        return;
                    }
                    webEvents.iterator().forEachRemaining(webEvent -> {
                        if (webEvent.getType().equals(WebEvent.TYPE_ACTIVITY_FINISH)) {
                            Intent intent = getIntent();
                            if (intent.hasExtra(Constants.INTENT_MANUAL_FOR_RESULT_DATA)) {
                                startActivity(intent.getParcelableExtra(Constants.INTENT_MANUAL_FOR_RESULT_DATA));
                            }
                            setResult(RESULT_OK);
                            finish();
                        } else if (webEvent.getType().equals(WebEvent.TYPE_VIEW_TARGET_FOR_ANNOTATION)) {
                            try {
                                JSONObject data = webEvent.getData();
                                String targetId = data.getString("targetId");
                                Annotation annotation = Annotation.fromJson(data.getJSONObject("annotation"));
                                boolean displayBottomSheet = data.getBoolean("displayBottomSheet");

                                sourceWebViewBottomSheetFragment.handleViewTargetForAnnotation(annotation, targetId)
                                        .ifPresent((s) -> {
                                            if (displayBottomSheet) {
                                                sourceWebViewViewModelBottomSheet.getBottomSheetBehavior().ifPresent(b -> b.setState(BottomSheetBehavior.STATE_EXPANDED));

                                                SourceInitializationStatus sourceInitializationStatus = sourceWebViewViewModelBottomSheet.getSourceInitializationStatus().getValue();
                                                // Source wil already be initialized if it was previously accessed or it may have loaded in a previous call where `displayBottomSheet` was false.
                                                if (sourceInitializationStatus.equals(SourceInitializationStatus.INITIALIZED)) {
                                                    appWebViewViewModelBottomSheet.setBottomSheetState(BottomSheetBehavior.STATE_COLLAPSED);
                                                    boolean javascriptEnabled = Optional.ofNullable(sourceWebViewViewModelBottomSheet).map((vm) -> vm.getSourceJavaScriptConfig().getValue().isEnabled()).orElse(true);
                                                    if (!javascriptEnabled) {
                                                        ToastRepository.show(this, R.string.toast_javascript_disabled, ToastRepository.STYLE_DARK_ACCENT);
                                                    }
                                                } else if (sourceInitializationStatus.equals(SourceInitializationStatus.FAILED)) {
                                                    ToastRepository.show(this, R.string.toast_annotation_renderer_failed_to_initialize, ToastRepository.STYLE_DARK_ACCENT);
                                                } else {
                                                    if (sourceInitializationStatusObserver != null) {
                                                        sourceWebViewViewModelBottomSheet.getSourceInitializationStatus().removeObserver(sourceInitializationStatusObserver);
                                                    }
                                                    sourceInitializationStatusObserver = new Observer<SourceInitializationStatus>() {
                                                        @Override
                                                        public void onChanged(SourceInitializationStatus sourceInitializationStatus) {
                                                            int sourceWebViewBottomSheetState = sourceWebViewViewModelBottomSheet.getBottomSheetBehavior()
                                                                    .map(BottomSheetBehavior::getState)
                                                                    .orElse(BottomSheetBehavior.STATE_HIDDEN);

                                                            if (sourceInitializationStatus.equals(SourceInitializationStatus.INITIALIZED) && sourceWebViewBottomSheetState != BottomSheetBehavior.STATE_HIDDEN) {
                                                                appWebViewViewModelBottomSheet.setBottomSheetState(BottomSheetBehavior.STATE_COLLAPSED);
                                                            }

                                                            if (sourceInitializationStatus.equals(SourceInitializationStatus.INITIALIZED) || sourceInitializationStatus.equals(SourceInitializationStatus.FAILED)) {
                                                                sourceWebViewViewModelBottomSheet.getSourceInitializationStatus().removeObserver(this);
                                                            }
                                                        }
                                                    };
                                                    sourceWebViewViewModelBottomSheet.getSourceInitializationStatus().observe(this, sourceInitializationStatusObserver);
                                                }
                                            }
                                        });
                            } catch (JSONException e) {
                                ErrorRepository.captureException(e, webEvent.toString());
                            }
                        } else if (webEvent.getType().equals(WebEvent.TYPE_CREATE_ANNOTATION_FROM_SOURCE)) {
                            this.handleCreateAnnotationFromSource(null);
                        }
                    });
                    appWebViewModelPrimary.clearReceivedWebEvents();
                }
        );

        sourceWebViewViewModelBottomSheet = new ViewModelProvider(this).get(SourceWebViewViewModel.class);

        appWebViewViewModelBottomSheet = new ViewModelProvider(this).get(APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME, AppWebViewViewModel.class);
        appWebViewViewModelBottomSheet.setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);
        appWebViewViewModelBottomSheet.getBottomSheetState().observe(this, bottomSheetState -> {
            if (bottomSheetState == BottomSheetBehavior.STATE_EXPANDED) {
                sourceWebViewViewModelBottomSheet.getBottomSheetBehavior()
                        .ifPresent(b -> b.setState(BottomSheetBehavior.STATE_EXPANDED));
            }
            AppWebViewBottomSheetAnimator.handleBottomSheetStateChange(
                    findViewById(R.id.app_web_view_bottom_sheet_fragment_container),
                    sourceWebViewViewModelBottomSheet.getFocusedAnnotation().map(SourceWebViewAnnotation::getAnnotation).orElse(null),
                    getResources(),
                    bottomSheetState,
                    (_e, webEvent) -> appWebViewBottomSheetFragment.postWebEvent(webEvent)
            );
        });

        return userFuture;
    }

    private void commitFragments(Bundle savedInstanceState, String initialUrl, User user) {
        if (getLifecycle().getCurrentState().equals(Lifecycle.State.DESTROYED)) {
            ErrorRepository.captureWarning(new Exception("Attempted to commitFragments when activity is DESTROYED, noop-ing."));
        }

        if (savedInstanceState != null) {
            appWebViewPrimaryFragment = (AppWebView) getSupportFragmentManager().getFragment(savedInstanceState, APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME);
            appWebViewBottomSheetFragment = (AppWebView) getSupportFragmentManager().getFragment(savedInstanceState, APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME);
            sourceWebViewBottomSheetFragment = (SourceWebView) getSupportFragmentManager().getFragment(savedInstanceState, SOURCE_WEB_VIEW_FRAGMENT_NAME);
        }

        if (appWebViewPrimaryFragment == null) {
            appWebViewPrimaryFragment = AppWebView.newInstance(initialUrl, APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME);
        }
        if (sourceWebViewBottomSheetFragment == null) {
            sourceWebViewBottomSheetFragment = SourceWebView.newInstance(
                    null,
                    SourceWebView.SourceContext.READ_SOURCE,
                    APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME,
                    APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME
            );
        }
        if (appWebViewBottomSheetFragment == null) {
            appWebViewBottomSheetFragment = AppWebView.newInstance(
                    user.getState().equals(UserState.SIGNED_IN) || user.getState().equals(UserState.GUEST)
                        ? WebRoutes.creatorsIdWebview(user.getEncodedIdentityId())
                        : null,
                    APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME
            );
        }

        FrameLayout bottomSheetBehaviorContainer = findViewById(R.id.source_web_view_bottom_sheet_behavior_container);
        BottomSheetBehavior<FrameLayout> sourceWebViewBottomSheetBehavior = BottomSheetBehavior.from(bottomSheetBehaviorContainer);
        sourceWebViewBottomSheetBehavior.setState(BottomSheetBehavior.STATE_HIDDEN);
        sourceWebViewBottomSheetBehavior.addBottomSheetCallback(new SourceWebViewBottomSheetCallback((_e, _d) -> {
            if (sourceWebViewViewModelBottomSheet.getFocusedAnnotation().isPresent()) {
                sourceWebViewBottomSheetFragment.handleAnnotationBlur();
            }
        }));
        sourceWebViewViewModelBottomSheet.setBottomSheetBehavior(Optional.of(sourceWebViewBottomSheetBehavior));

        sourceWebViewBottomSheetFragment.setOnToolbarPrimaryActionCallback((createdAnnotations, _source) -> {
            this.handleSourceWebViewBottomSheetHidden(createdAnnotations);
            return null;
        });
        sourceWebViewBottomSheetFragment.setOnCreateAnnotationFromSource((_e, sourceUrl) -> {
            this.handleCreateAnnotationFromSource(sourceUrl.toString());
        });

        FragmentTransaction fragmentTransaction = getSupportFragmentManager()
                .beginTransaction()
                .setReorderingAllowed(true);

        if (!appWebViewPrimaryFragment.isAdded()) {
            fragmentTransaction.add(R.id.fragment_container, appWebViewPrimaryFragment);
        }
        if (!sourceWebViewBottomSheetFragment.isAdded()) {
            fragmentTransaction.add(R.id.source_web_view_bottom_sheet_fragment_container, sourceWebViewBottomSheetFragment);
        }
        if (!appWebViewBottomSheetFragment.isAdded()) {
            fragmentTransaction.add(R.id.app_web_view_bottom_sheet_fragment_container, appWebViewBottomSheetFragment);
        }

        if (!fragmentTransaction.isEmpty()) {
            fragmentTransaction.commit();
        }
    }

    private void handleCreateAnnotationFromSource(String sourceUrl) {
        Intent intent = new Intent(this, ShareTargetHandler.class);
        intent.setAction(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_TEXT, sourceUrl != null ? sourceUrl : "https://google.com");

        createAnnotationFromSourceLauncher.launch(intent);
    }

    private void handleSourceWebViewBottomSheetHidden(Annotation[] createdAnnotations) {
        sourceWebViewViewModelBottomSheet.getBottomSheetBehavior().ifPresent(b -> b.setState(BottomSheetBehavior.STATE_HIDDEN));
        try {
            String json = JsonArrayUtil.stringifyObjectArray(createdAnnotations, Annotation::toJson).toString();
            JSONObject addCacheAnnotationsData = new JSONObject();
            addCacheAnnotationsData.put("annotations", json);
            appWebViewPrimaryFragment.postWebEvent(new WebEvent(
                    WebEvent.TYPE_ADD_CACHE_ANNOTATIONS,
                    UUID.randomUUID().toString(),
                    addCacheAnnotationsData
            ));
            Annotation[] annotations;
            try {
                annotations = JsonArrayUtil.parseJsonObjectArray(new JSONArray(json), new Annotation[0], Annotation::fromJson);
            } catch (JSONException e) {
                annotations = new Annotation[0];
            }
            if (annotations.length == 1) {
                ToastRepository.show(this, R.string.toast_annotation_created, ToastRepository.STYLE_DARK_ACCENT);
            } else if (annotations.length > 1) {
                ToastRepository.show(this, R.string.toast_annotations_created, ToastRepository.STYLE_DARK_ACCENT);
            }
        } catch (JSONException e) {
            ErrorRepository.captureException(e);
        }
    }

    private void handleCreateAnnotationFromSourceResult(ActivityResult result) {
        Intent data = result.getData();
        if (result.getResultCode() == ShareTargetHandler.RESULT_OK && data != null) {
            sourceWebViewViewModelBottomSheet.getBottomSheetBehavior().ifPresent(b -> b.setState(BottomSheetBehavior.STATE_HIDDEN));
            appWebViewViewModelBottomSheet.setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);
            String json = data.getStringExtra(ShareTargetHandler.RESULT_EXTRA_ANNOTATIONS);
            try {
                JSONObject addCacheAnnotationsData = new JSONObject();
                addCacheAnnotationsData.put("annotations", json);
                appWebViewPrimaryFragment.postWebEvent(new WebEvent(
                        WebEvent.TYPE_ADD_CACHE_ANNOTATIONS,
                        UUID.randomUUID().toString(),
                        addCacheAnnotationsData
                ));
                Annotation[] annotations;
                try {
                    annotations = JsonArrayUtil.parseJsonObjectArray(new JSONArray(json), new Annotation[0], Annotation::fromJson);
                } catch (JSONException e) {
                    annotations = new Annotation[0];
                }
                if (annotations.length == 1) {
                    ToastRepository.show(this, R.string.toast_annotation_created, ToastRepository.STYLE_DARK_ACCENT);
                } else if (annotations.length > 1) {
                    ToastRepository.show(this, R.string.toast_annotations_created, ToastRepository.STYLE_DARK_ACCENT);
                }
            } catch (JSONException e) {
                ErrorRepository.captureException(e);
            }
        }
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);

        FragmentManager fragmentManager = getSupportFragmentManager();
        if (appWebViewPrimaryFragment != null) {
            fragmentManager.putFragment(outState, APP_WEB_VIEW_PRIMARY_FRAGMENT_NAME, appWebViewPrimaryFragment);
        }
        if (sourceWebViewBottomSheetFragment != null) {
            fragmentManager.putFragment(outState, SOURCE_WEB_VIEW_FRAGMENT_NAME, sourceWebViewBottomSheetFragment);
        }
        if (appWebViewBottomSheetFragment != null) {
            fragmentManager.putFragment(outState, APP_WEB_VIEW_BOTTOM_SHEET_FRAGMENT_NAME, appWebViewBottomSheetFragment);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == AuthClient.CUSTOM_TABS_ACTIVITY_CODE) {
            AWSMobileClient.getInstance().handleAuthResponse(data);
        }
    }

    @Override
    public void onBackPressed() {
        Optional<BottomSheetBehavior<FrameLayout>> bottomSheetBehavior = sourceWebViewViewModelBottomSheet.getBottomSheetBehavior();
        if (bottomSheetBehavior.isPresent()) {
            if (bottomSheetBehavior.get().getState() == BottomSheetBehavior.STATE_EXPANDED ||
                    bottomSheetBehavior.get().getState() == BottomSheetBehavior.STATE_COLLAPSED) {
                if (sourceWebViewBottomSheetFragment.getWebView().handleBackPressed()) {
                    return;
                }
                bottomSheetBehavior.get().setState(BottomSheetBehavior.STATE_HIDDEN);
                return;
            }
            if (appWebViewPrimaryFragment.getWebView().handleBackPressed()) {
                return;
            }
        }
        super.onBackPressed();
    }

    private class SourceWebViewBottomSheetCallback extends BottomSheetBehavior.BottomSheetCallback {

        private Float prevSlideOffset;
        private Callback<Void, Void> onSlideBelowThreshold;

        public SourceWebViewBottomSheetCallback(Callback<Void, Void> onSlideBelowThreshold) {
            super();
            prevSlideOffset = null;
            this.onSlideBelowThreshold = onSlideBelowThreshold;
        }

        @Override
        public void onStateChanged(@NonNull View bottomSheet, int newState) {
            /** noop **/
        }

        @Override
        public void onSlide(@NonNull View bottomSheet, float slideOffset) {
            if ((prevSlideOffset == null || slideOffset < prevSlideOffset) && slideOffset < -.25) {
                onSlideBelowThreshold.invoke(null, null);
            }
            prevSlideOffset = slideOffset;
        }
    }
}
