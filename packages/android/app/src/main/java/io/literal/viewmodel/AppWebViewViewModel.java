package io.literal.viewmodel;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;

import io.literal.lib.WebEvent;
import io.literal.ui.activity.ShareTargetHandler;
import io.literal.ui.view.WebView;

public class AppWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<OnFinishCallback> onFinishedCallback = new MutableLiveData<>(null);
    private final MutableLiveData<Integer> bottomSheetState = new MutableLiveData<>(null);
    private final MutableLiveData<String> url = new MutableLiveData<>(null);

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public void setOnFinishCallback(OnFinishCallback callback) {
        this.onFinishedCallback.setValue(callback);
    }

    public MutableLiveData<Boolean> getHasFinishedInitializing() { return hasFinishedInitializing; }

    public MutableLiveData<String> getUrl() {
        return url;
    }

    public void setUrl(String url) {
        this.url.setValue(url);
    }

    public MutableLiveData<Integer> getBottomSheetState() {
        return bottomSheetState;
    }

    public void setBottomSheetState(Integer state) {
        this.bottomSheetState.setValue(state);
    }

    public interface OnFinishCallback {
        public void onFinish();
    }
}
