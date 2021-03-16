package io.literal.ui.activity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import androidx.activity.result.ActivityResult;
import androidx.activity.result.ActivityResultCallback;
import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.FragmentManager;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationFromExternalTargetMutation;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobileconnectors.cognitoauth.AuthClient;
import com.google.android.material.bottomsheet.BottomSheetBehavior;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.UUID;

import javax.xml.transform.Source;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.model.Annotation;
import io.literal.model.Target;
import io.literal.repository.ShareTargetHandlerRepository;
import io.literal.ui.MainApplication;
import io.literal.ui.fragment.AppWebView;
import io.literal.ui.fragment.SourceWebView;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;

public class MainActivity extends AppCompatActivity {

    private AppWebViewViewModel appWebViewModel;
    private AuthenticationViewModel authenticationViewModel;

    private AppWebView appWebViewFragment;
    private SourceWebView sourceWebViewFragment;
    private final ActivityResultLauncher<Intent> createAnnotationFromSourceLauncher = registerForActivityResult(new ActivityResultContracts.StartActivityForResult(), result -> MainActivity.this.handleCreateAnnotationFromSourceResult(result));

    private static final String APP_WEB_VIEW_FRAGMENT_NAME = "MAIN_ACTIVITY_APP_WEB_VIEW_FRAGMENT";
    private static final String SOURCE_WEB_VIEW_FRAGMENT_NAME = "MAIN_ACTIVITY_SOURCE_WEB_VIEW_FRAGMENT";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        this.initializeViewModel();

        if (savedInstanceState == null) {
            authenticationViewModel.awaitInitialization(
                    ((MainApplication) getApplication()).getThreadPoolExecutor(),
                    (e, aVoid) -> runOnUiThread(() -> {
                        String initialUrl;
                        String username = authenticationViewModel.getUsername().getValue();
                        String defaultUrl =
                                username != null
                                        ? WebRoutes.creatorsIdAnnotationCollectionId(username, Constants.RECENT_ANNOTATION_COLLECTION_ID_COMPONENT)
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
                        this.commitFragments(null, initialUrl);
                    })
            );
        } else {
            this.commitFragments(savedInstanceState, null);
        }
    }

    private void initializeViewModel() {
        authenticationViewModel = new ViewModelProvider(this).get(AuthenticationViewModel.class);
        authenticationViewModel.initialize(this);

        appWebViewModel = new ViewModelProvider(this).get(AppWebViewViewModel.class);
        appWebViewModel.getHasFinishedInitializing().observe(this, hasFinishedInitializing -> {
            ViewGroup splash = findViewById(R.id.splash);
            splash.setVisibility(hasFinishedInitializing ? View.INVISIBLE : View.VISIBLE);
        });
        appWebViewModel.getReceivedWebEvents().observe(this,
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

                                sourceWebViewFragment.handleViewTargetForAnnotation(annotation, targetId);
                                if (displayBottomSheet) {
                                    this.setBottomSheetState(BottomSheetBehavior.STATE_EXPANDED);
                                }
                            } catch (JSONException e) {
                                Log.d("MainActivity", "Unable to handle event: " + webEvent.toString(), e);
                            }
                        } else if (webEvent.getType().equals(WebEvent.TYPE_CREATE_ANNOTATION_FROM_SOURCE)) {
                            this.handleCreateAnnotationFromSource();
                        }
                    });
                    appWebViewModel.clearReceivedWebEvents();
                }
        );
    }

    private void commitFragments(Bundle savedInstanceState, String initialUrl) {
        if (savedInstanceState != null) {
            appWebViewFragment = (AppWebView) getSupportFragmentManager().getFragment(savedInstanceState, APP_WEB_VIEW_FRAGMENT_NAME);
            sourceWebViewFragment = (SourceWebView) getSupportFragmentManager().getFragment(savedInstanceState, SOURCE_WEB_VIEW_FRAGMENT_NAME);
        } else {
            appWebViewFragment = AppWebView.newInstance(initialUrl);
            sourceWebViewFragment = SourceWebView.newInstance(null, false);
        }

        setBottomSheetState(BottomSheetBehavior.STATE_HIDDEN);

        getSupportFragmentManager()
                .beginTransaction()
                .setReorderingAllowed(true)
                .add(R.id.fragment_container, appWebViewFragment)
                .add(R.id.bottom_sheet_fragment_container, sourceWebViewFragment)
                .commit();
    }

    private void handleCreateAnnotationFromSource() {
        Intent intent = new Intent(this, ShareTargetHandler.class);
        intent.setAction(Intent.ACTION_SEND);
        intent.setType("text/plain");
        intent.putExtra(Intent.EXTRA_TEXT, "https://google.com");

        createAnnotationFromSourceLauncher.launch(intent);
    }

    private void handleCreateAnnotationFromSourceResult(ActivityResult result) {
        Intent data = result.getData();
        Log.i("MainActivity", "handleCreateAnnotationFromSourceResult: " + result.getResultCode());
        if (result.getResultCode() == ShareTargetHandler.RESULT_OK && data != null) {
            String json = data.getStringExtra(ShareTargetHandler.RESULT_EXTRA_ANNOTATIONS);
            try {
                JSONObject addCacheAnnotationsData = new JSONObject();
                addCacheAnnotationsData.put("annotations", json);
                appWebViewFragment.postWebEvent(new WebEvent(
                        WebEvent.TYPE_ADD_CACHE_ANNOTATIONS,
                        UUID.randomUUID().toString(),
                        addCacheAnnotationsData
                ));
            } catch (JSONException e) {
                Log.d("MainActivity", "Unable to dispatch ADD_CACHE_ANNOTATIONS", e);
            }
        }
    }

    private void setBottomSheetState(int bottomSheetState) {
        FrameLayout bottomSheetBehaviorContainer = findViewById(R.id.bottom_sheet_behavior_container);
        BottomSheetBehavior<FrameLayout> behavior = BottomSheetBehavior.from(bottomSheetBehaviorContainer);
        behavior.setState(bottomSheetState);
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);

        FragmentManager fragmentManager = getSupportFragmentManager();
        fragmentManager.putFragment(outState, APP_WEB_VIEW_FRAGMENT_NAME, appWebViewFragment);
        fragmentManager.putFragment(outState, SOURCE_WEB_VIEW_FRAGMENT_NAME, sourceWebViewFragment);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == AuthClient.CUSTOM_TABS_ACTIVITY_CODE) {
            AWSMobileClient.getInstance().handleAuthResponse(data);
        }
    }
}
