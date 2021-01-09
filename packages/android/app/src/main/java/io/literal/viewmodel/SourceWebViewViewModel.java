package io.literal.viewmodel;

import android.content.res.AssetManager;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.apache.commons.io.IOUtils;

import java.io.IOException;
import java.nio.charset.StandardCharsets;

public class SourceWebViewViewModel extends ViewModel {
    private MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private MutableLiveData<String> injectedGetSelectionScript = new MutableLiveData<>(null);

    private static final String INJECTED_GET_SELECTION_SCRIPT_NAME = "SourceWebViewGetSelection.js";

    public MutableLiveData<Boolean> getHasFinishedInitializing() {
        return hasFinishedInitializing;
    }

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public MutableLiveData<String> getInjectedGetSelectionScript(AssetManager assetManager) {
        if (injectedGetSelectionScript.getValue() == null) {
            try {
                injectedGetSelectionScript.setValue(
                        IOUtils.toString(assetManager.open(INJECTED_GET_SELECTION_SCRIPT_NAME), StandardCharsets.UTF_8)
                );
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "getInjectedGetSelectionScript", e);
            }
        }
        return injectedGetSelectionScript;
    }

    public void createAnnotationFromSelection(String json) {
        Log.d("SourceWebViewViewModel", "createAnnotationFromSelection: " + json);
    }
}
