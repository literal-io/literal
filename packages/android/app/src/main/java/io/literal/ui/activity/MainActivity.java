package io.literal.ui.activity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProvider;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationFromExternalTargetMutation;
import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobile.client.Callback;
import com.amazonaws.mobile.client.UserState;
import com.amazonaws.mobile.client.UserStateDetails;
import com.amazonaws.mobileconnectors.cognitoauth.AuthClient;

import io.literal.R;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.lib.Constants;
import io.literal.lib.WebEvent;
import io.literal.lib.WebRoutes;
import io.literal.repository.ShareTargetHandlerRepository;
import io.literal.ui.MainApplication;
import io.literal.ui.fragment.AppWebView;
import io.literal.viewmodel.AppWebViewViewModel;
import io.literal.viewmodel.AuthenticationViewModel;

public class MainActivity extends AppCompatActivity {

    private AppWebViewViewModel appWebViewModel;
    private AuthenticationViewModel authenticationViewModel;

    private AppWebView appWebViewFragment;

    private static final String APP_WEB_VIEW_FRAGMENT_NAME = "APP_WEB_VIEW_FRAGMENT";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

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
                        setResult(RESULT_OK);
                        finish();
                    }
                });
                appWebViewModel.clearReceivedWebEvents();
            }
        );

        if (savedInstanceState == null) {
            ((MainApplication) getApplication()).getThreadPoolExecutor().execute(() -> {
                authenticationViewModel.awaitInitialization();

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

                appWebViewFragment = AppWebView.newInstance(initialUrl);
                getSupportFragmentManager()
                        .beginTransaction()
                        .setReorderingAllowed(true)
                        .add(R.id.fragment_container, appWebViewFragment)
                        .commit();
            });
        } else {
            appWebViewFragment = (AppWebView) getSupportFragmentManager().getFragment(savedInstanceState, APP_WEB_VIEW_FRAGMENT_NAME);
            getSupportFragmentManager()
                    .beginTransaction()
                    .setReorderingAllowed(true)
                    .add(R.id.fragment_container, appWebViewFragment)
                    .commit();
        }
    }

    @Override
    protected void onSaveInstanceState(@NonNull Bundle outState) {
        super.onSaveInstanceState(outState);

        getSupportFragmentManager().putFragment(outState, APP_WEB_VIEW_FRAGMENT_NAME, appWebViewFragment);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == AuthClient.CUSTOM_TABS_ACTIVITY_CODE) {
            AWSMobileClientFactory.initializeClient(this, new Callback<UserStateDetails>() {
                @Override
                public void onResult(UserStateDetails result) {
                    AWSMobileClient.getInstance().handleAuthResponse(data);
                }
                @Override
                public void onError(Exception e) {
                    Log.e(Constants.LOG_TAG, "Unable to initializeClient: ", e);
                }
            });
        }
    }
}
