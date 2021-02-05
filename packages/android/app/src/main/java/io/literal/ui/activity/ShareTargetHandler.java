package io.literal.ui.activity;

import android.animation.Animator;
import android.animation.ObjectAnimator;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.transition.ChangeBounds;
import android.transition.TransitionManager;
import android.transition.TransitionSet;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.Patterns;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.Animation;
import android.widget.FrameLayout;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentContainerView;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationFromExternalTargetMutation;
import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;
import com.apollographql.apollo.api.Error;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.json.JSONException;

import java.util.List;
import java.util.UUID;
import java.util.function.Consumer;

import io.literal.R;
import io.literal.lib.Constants;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.repository.ShareTargetHandlerRepository;
import io.literal.ui.fragment.AppWebView;
import io.literal.ui.fragment.SourceWebView;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;
import io.literal.viewmodel.SourceWebViewViewModel;

public class ShareTargetHandler extends AppCompatActivity {

    private static final String APP_WEB_VIEW_FRAGMENT_NAME = "APP_WEB_VIEW_FRAGMENT";
    private static final String SOURCE_WEB_VIEW_FRAGMENT_NAME = "SOURCE_WEB_VIEW_FRAGMENT";
    private AppWebViewViewModel appWebViewViewModel;
    private SourceWebViewViewModel sourceWebViewViewModel;
    private AuthenticationViewModel authenticationViewModel;
    private AppWebView appWebViewFragment;
    private SourceWebView sourceWebViewFragment;
    private FragmentContainerView bottomSheetFragmentContainer;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        authenticationViewModel = new ViewModelProvider(this).get(AuthenticationViewModel.class);
        authenticationViewModel.initialize(this);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel chan = new NotificationChannel(
                    Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_ID,
                    Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_NAME,
                    NotificationManager.IMPORTANCE_LOW
            );
            chan.setDescription(Constants.NOTIFICATION_CHANNEL_ANNOTATION_CREATED_DESCRIPTION);
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            if (notificationManager != null) {
                notificationManager.createNotificationChannel(chan);
            }
        }

        if (savedInstanceState == null) {
            Intent intent = getIntent();
            String action = intent.getAction();
            String type = intent.getType();

            if (Intent.ACTION_SEND.equals(action) && type != null && type.startsWith("image/")) {
                handleCreateFromImage(intent);
            } else if (Intent.ACTION_SEND.equals(action) && type != null && type.equals("text/plain")) {
                String text = intent.getStringExtra(Intent.EXTRA_TEXT);
                if (Patterns.WEB_URL.matcher(text).matches()) {
                    handleCreateFromSource(intent);
                } else {
                    handleCreateFromText(intent);
                }
            } else {
                handleSendNotSupported();
            }
        } else {
            Fragment fragment;
            fragment = getSupportFragmentManager().getFragment(savedInstanceState, APP_WEB_VIEW_FRAGMENT_NAME);
            if (fragment == null) {
                fragment = getSupportFragmentManager().getFragment(savedInstanceState, SOURCE_WEB_VIEW_FRAGMENT_NAME);
            }
            if (fragment != null) {
                getSupportFragmentManager()
                        .beginTransaction()
                        .setReorderingAllowed(true)
                        .add(R.id.fragment_container, fragment)
                        .commit();
            }
        }
    }


    private void installSourceWebView(String sourceWebViewUri, String appWebViewUri) {
        sourceWebViewViewModel = new ViewModelProvider(this).get(SourceWebViewViewModel.class);
        sourceWebViewViewModel.getHasFinishedInitializing().observe(this, hasFinishedInitializing -> {
            ViewGroup splash = findViewById(R.id.share_target_handler_splash);
            splash.setVisibility(hasFinishedInitializing ? View.INVISIBLE : View.VISIBLE);
        });

        bottomSheetFragmentContainer = findViewById(R.id.bottom_sheet_fragment_container);

        // initialize view model for managing app web view bottom sheet
        appWebViewViewModel = new ViewModelProvider(this).get(AppWebViewViewModel.class);
        appWebViewViewModel.getBottomSheetState().observe(this, bottomSheetState -> {
            Annotation focusedAnnotation = sourceWebViewViewModel.getFocusedAnnotation().getValue();
            DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
            Configuration configuration = getResources().getConfiguration();

            if (bottomSheetState == BottomSheetBehavior.STATE_EXPANDED) {
                double targetBottomSheetHeightPx = configuration.screenHeightDp * 0.8 * displayMetrics.density;
                double targetTranslationY = 0;
                double initialTranslationY = targetBottomSheetHeightPx - Math.abs(bottomSheetFragmentContainer.getTranslationY());

                ViewGroup.LayoutParams bottomSheetLayout = bottomSheetFragmentContainer.getLayoutParams();
                bottomSheetLayout.height = (int) targetBottomSheetHeightPx;
                bottomSheetFragmentContainer.setLayoutParams(bottomSheetLayout);
                bottomSheetFragmentContainer.setTranslationY((float) initialTranslationY);

                try {
                    WebEvent webEvent = new WebEvent(
                            WebEvent.TYPE_VIEW_STATE_EDIT_ANNOTATION_TAGS,
                            UUID.randomUUID().toString(),
                            focusedAnnotation.toJson()
                    );
                    appWebViewViewModel.dispatchWebEvent(webEvent);
                } catch (JSONException ex) {
                    Log.d("ShareTargetHandler", "Unable to serialize annotation", ex);
                }

                ObjectAnimator animator = ObjectAnimator.ofFloat(bottomSheetFragmentContainer, "translationY", (float) initialTranslationY, (float) targetTranslationY);
                animator.setDuration(300);
                animator.start();
            } else if (bottomSheetState == BottomSheetBehavior.STATE_COLLAPSED) {
                double targetBottomSheetHeightPx =  96 * displayMetrics.density;
                double targetTranslationY;
                double initialTranslationY;
                Animator.AnimatorListener animatorListener = null;

                if (bottomSheetFragmentContainer.getHeight() == 0) {
                    // animating from hidden state
                    initialTranslationY = targetBottomSheetHeightPx;
                    targetTranslationY = 0;

                    ViewGroup.LayoutParams bottomSheetLayout = bottomSheetFragmentContainer.getLayoutParams();
                    bottomSheetLayout.height = (int) targetBottomSheetHeightPx;
                    bottomSheetFragmentContainer.setLayoutParams(bottomSheetLayout);
                    bottomSheetFragmentContainer.setTranslationY((float) initialTranslationY);
                } else {
                    // animating from expanded state
                    targetTranslationY = bottomSheetFragmentContainer.getHeight() - targetBottomSheetHeightPx;
                    initialTranslationY = bottomSheetFragmentContainer.getTranslationY();
                    animatorListener = new Animator.AnimatorListener() {
                        @Override
                        public void onAnimationEnd(Animator animation) {
                            ViewGroup.LayoutParams bottomSheetLayout = bottomSheetFragmentContainer.getLayoutParams();
                            bottomSheetLayout.height = (int) targetBottomSheetHeightPx;
                            bottomSheetFragmentContainer.setLayoutParams(bottomSheetLayout);
                            bottomSheetFragmentContainer.setTranslationY(0);
                        }

                        @Override
                        public void onAnimationStart(Animator animation) {
                            // noop
                        }

                        @Override
                        public void onAnimationCancel(Animator animation) {
                            // noop
                        }

                        @Override
                        public void onAnimationRepeat(Animator animation) {
                            // noop
                        }
                    };
                }

                try {
                    WebEvent webEvent = new WebEvent(
                            WebEvent.TYPE_VIEW_STATE_COLLAPSED_ANNOTATION_TAGS,
                            UUID.randomUUID().toString(),
                            focusedAnnotation.toJson()
                    );
                    appWebViewViewModel.dispatchWebEvent(webEvent);
                } catch (JSONException ex) {
                    Log.d("ShareTargetHandler", "Unable to serialize annotation", ex);
                }

                ObjectAnimator animator = ObjectAnimator.ofFloat(bottomSheetFragmentContainer, "translationY", (float) initialTranslationY, (float) targetTranslationY);
                if (animatorListener != null) {
                    animator.addListener(animatorListener);
                }
                animator.setDuration(300);
                animator.start();

            } else if (bottomSheetState == BottomSheetBehavior.STATE_HIDDEN) {
                double targetBottomSheetHeightPx =  0;
                double targetTranslationY = bottomSheetFragmentContainer.getHeight();
                double initialTranslationY = bottomSheetFragmentContainer.getTranslationY();

                ObjectAnimator animator = ObjectAnimator.ofFloat(bottomSheetFragmentContainer, "translationY", (float) initialTranslationY, (float) targetTranslationY);
                animator.addListener(new Animator.AnimatorListener() {

                    @Override
                    public void onAnimationEnd(Animator animation) {
                        ViewGroup.LayoutParams bottomSheetLayout = bottomSheetFragmentContainer.getLayoutParams();
                        bottomSheetLayout.height = (int) targetBottomSheetHeightPx;
                        bottomSheetFragmentContainer.setLayoutParams(bottomSheetLayout);
                        bottomSheetFragmentContainer.setTranslationY(0);
                    }

                    @Override
                    public void onAnimationStart(Animator animation) {
                        // Noop
                    }

                    @Override
                    public void onAnimationCancel(Animator animation) {
                        // Noop
                    }

                    @Override
                    public void onAnimationRepeat(Animator animation) {
                        // Noop
                    }
                });
                animator.start();
            }
        });

        appWebViewViewModel.setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);

        sourceWebViewFragment = SourceWebView.newInstance(sourceWebViewUri);
        appWebViewFragment = AppWebView.newInstance(appWebViewUri);
        getSupportFragmentManager()
                .beginTransaction()
                .setReorderingAllowed(true)
                .add(R.id.fragment_container, sourceWebViewFragment)
                .add(R.id.bottom_sheet_fragment_container, appWebViewFragment)
                .commit();
    }

    private void installAppWebView(String appWebViewUri) {
        // Remove unused bottom sheet fragment container
        ViewGroup layout = findViewById(R.id.layout);
        View bottomSheetContainer = findViewById(R.id.bottom_sheet_fragment_container);
        if (layout != null && bottomSheetContainer != null) {
            layout.removeView(bottomSheetContainer);
        }

        appWebViewViewModel = new ViewModelProvider(this).get(AppWebViewViewModel.class);
        appWebViewViewModel.getHasFinishedInitializing().observe(this, hasFinishedInitializing -> {
            ViewGroup splash = findViewById(R.id.share_target_handler_splash);
            splash.setVisibility(hasFinishedInitializing ? View.INVISIBLE : View.VISIBLE);
        });
        appWebViewFragment = AppWebView.newInstance(appWebViewUri);
        getSupportFragmentManager()
                .beginTransaction()
                .setReorderingAllowed(true)
                .add(R.id.fragment_container, appWebViewFragment)
                .commit();
    }

    private void handleCreateFromSource(Intent intent) {
        authenticationViewModel.awaitInitialization();
        String sourceWebViewUri = intent.getStringExtra(Intent.EXTRA_TEXT);
        String appWebViewUri = WebRoutes.creatorsIdWebview(
                authenticationViewModel.getUsername().getValue()
        );
        installSourceWebView(sourceWebViewUri, appWebViewUri);
    }

    private void handleCreateFromText(Intent intent) {
        String text = intent.getStringExtra(Intent.EXTRA_TEXT);
        authenticationViewModel.awaitInitialization();
        ShareTargetHandlerRepository.createAnnotationFromText(text, this, authenticationViewModel, new ShareTargetHandlerRepository.CreateListener<CreateAnnotationMutation.Data>() {
            @Override
            public void onAnnotationUri(String uri) {
                installAppWebView(uri);
            }

            @Override
            public void onAnnotationCreated(CreateAnnotationMutation.Data data) {
                if (data == null) {
                    // Error is handled in WebView
                    Log.d("handleSendFromText", "CreateAnnotationMutation.Data is null");
                    return;
                }

                runOnUiThread(() -> {
                    appWebViewViewModel.getReceivedWebEvents().observe(ShareTargetHandler.this, (webEvents) -> {
                        if (webEvents == null) {
                            return;
                        }

                        webEvents.iterator().forEachRemaining(webEvent -> {
                            if (webEvent.getType().equals(WebEvent.TYPE_ACTIVITY_FINISH)) {
                                CreateAnnotationMutation.CreateAnnotation annotation = data.createAnnotation();
                                if (annotation != null) {
                                    ShareTargetHandlerRepository.displayAnnotationCreatedNotification(annotation.annotation().id(), ShareTargetHandler.this);
                                }
                                finish();
                            }
                        });

                        appWebViewViewModel.clearReceivedWebEvents();
                    });
                });
            }

            @Override
            public void onError(Exception e) {
                Log.d("ShareTargetHandlerGraphQLService", "onError", e);
            }

            @Override
            public void onGraphQLError(List<Error> errors) {
                errors.forEach(error -> Log.d("ShareTargetHandlerGraphQLService", "onGraphQLError - " + error.message()));
            }
        });
    }

    private void handleCreateFromImage(Intent intent) {
        Uri imageUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
        authenticationViewModel.awaitInitialization();
        ShareTargetHandlerRepository.createAnnotationFromImage(imageUri, this, authenticationViewModel, new ShareTargetHandlerRepository.CreateListener<CreateAnnotationFromExternalTargetMutation.Data>() {
            @Override
            public void onAnnotationUri(String uri) {
                installAppWebView(uri);
            }

            @Override
            public void onAnnotationCreated(CreateAnnotationFromExternalTargetMutation.Data data) {
                if (data == null) {
                    // Error is handled in WebView.CreateAnnotationMutation.Data
                    Log.i("handleCreateFromImage", "CreateAnnotationMutation.Data is null");
                    return;
                }

                runOnUiThread(() -> {
                    appWebViewViewModel.getReceivedWebEvents().observe(ShareTargetHandler.this, (webEvents) -> {
                        if (webEvents == null) {
                            return;
                        }

                        webEvents.iterator().forEachRemaining(webEvent -> {
                            if (webEvent.getType().equals(WebEvent.TYPE_ACTIVITY_FINISH)) {
                                CreateAnnotationFromExternalTargetMutation.CreateAnnotationFromExternalTarget annotation = data.createAnnotationFromExternalTarget();
                                if (annotation != null) {
                                    ShareTargetHandlerRepository.displayAnnotationCreatedNotification(annotation.id(), ShareTargetHandler.this);
                                }
                                finish();
                            }
                        });

                        appWebViewViewModel.clearReceivedWebEvents();
                    });
                });
            }

            @Override
            public void onError(Exception e) {
                Log.d("ShareTargetHandlerGraphQLService", "onError", e);
            }

            @Override
            public void onGraphQLError(List<Error> errors) {
                errors.forEach(new Consumer<Error>() {
                    @Override
                    public void accept(Error error) {
                        Log.d("ShareTargetHandlerGraphQLService", "onGraphQLError - " + error.message());
                    }
                });
            }
        });
    }

    private void handleSendNotSupported() {
        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);
        if (appWebViewFragment != null) {
            getSupportFragmentManager().putFragment(outState, APP_WEB_VIEW_FRAGMENT_NAME, appWebViewFragment);
        }
        if (sourceWebViewFragment != null) {
            getSupportFragmentManager().putFragment(outState, APP_WEB_VIEW_FRAGMENT_NAME, sourceWebViewFragment);
        }
    }
}
