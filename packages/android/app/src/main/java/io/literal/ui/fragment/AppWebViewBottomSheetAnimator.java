package io.literal.ui.fragment;

import android.animation.Animator;
import android.animation.ObjectAnimator;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.ViewGroup;

import androidx.fragment.app.FragmentContainerView;

import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.jetbrains.annotations.NotNull;
import org.json.JSONException;

import java.util.UUID;

import io.literal.lib.Callback;
import io.literal.lib.WebEvent;
import io.literal.model.Annotation;

public class AppWebViewBottomSheetAnimator {
    public static void handleBottomSheetStateChange(
            @NotNull FragmentContainerView bottomSheetFragmentContainer,
            Annotation focusedAnnotation,
            @NotNull Resources resources,
            Integer bottomSheetState,
            Callback<Void, WebEvent> dispatchWebEvent
    ) {
        if (bottomSheetState == null) {
            return;
        }

        DisplayMetrics displayMetrics = resources.getDisplayMetrics();
        Configuration configuration = resources.getConfiguration();

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
                dispatchWebEvent.invoke(null, webEvent);
            } catch (JSONException ex) {
                Log.d("ShareTargetHandler", "Unable to serialize annotation", ex);
            }

            ObjectAnimator animator = ObjectAnimator.ofFloat(bottomSheetFragmentContainer, "translationY", (float) initialTranslationY, (float) targetTranslationY);
            animator.setDuration(300);
            animator.start();
        } else if (bottomSheetState == BottomSheetBehavior.STATE_COLLAPSED) {
            double targetBottomSheetHeightPx = 48 * displayMetrics.density;
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
                dispatchWebEvent.invoke(null, webEvent);
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
            double targetBottomSheetHeightPx = 0;
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
    }
}
