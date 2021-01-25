package io.literal.viewmodel;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import com.amazonaws.amplify.generated.graphql.CreateAnnotationMutation;

import java.util.ArrayDeque;
import java.util.Deque;

import io.literal.lib.WebEvent;
import io.literal.ui.activity.ShareTargetHandler;
import io.literal.ui.view.WebView;

public class AppWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<Integer> bottomSheetState = new MutableLiveData<>(null);
    private final MutableLiveData<String> url = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayDeque<WebEvent>> webEvents = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayDeque<WebEvent>> receivedWebEvents = new MutableLiveData<>(null);

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public void dispatchWebEvent(WebEvent webEvent) {
        ArrayDeque<WebEvent> newWebEvents;
        if (webEvents.getValue() == null) {
            newWebEvents = new ArrayDeque<>();
        } else {
            newWebEvents = webEvents.getValue().clone();
        }
        newWebEvents.add(webEvent);
        webEvents.setValue(newWebEvents);
    }

    public void clearWebEvents() {
        webEvents.setValue(null);
    }

    public void dispatchReceivedWebEvent(WebEvent webEvent) {
        ArrayDeque<WebEvent> newReceivedWebEvents;
        if (webEvents.getValue() == null) {
            newReceivedWebEvents = new ArrayDeque<>();
        } else {
            newReceivedWebEvents = webEvents.getValue().clone();
        }
        newReceivedWebEvents.add(webEvent);
        receivedWebEvents.setValue(newReceivedWebEvents);
    }

    public void clearReceivedWebEvents() {
        receivedWebEvents.setValue(null);
    }

    public MutableLiveData<ArrayDeque<WebEvent>> getReceivedWebEvents() { return receivedWebEvents; }

    public MutableLiveData<ArrayDeque<WebEvent>> getWebEvents() { return webEvents; }

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
}
