package io.literal.viewmodel;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;

import io.literal.lib.WebEvent;
import io.literal.ui.activity.ShareTargetHandler;
import io.literal.ui.view.WebView;

public class AppWebViewViewModel extends ViewModel {
    private MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private MutableLiveData<OnFinishCallback> onFinishedCallback = new MutableLiveData<>(null);

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public void setOnFinishCallback(OnFinishCallback callback) {
        this.onFinishedCallback.setValue(callback);
    }

    public MutableLiveData<Boolean> getHasFinishedInitializing() { return hasFinishedInitializing; }

    public interface OnFinishCallback {
        public void onFinish();
    }
}
