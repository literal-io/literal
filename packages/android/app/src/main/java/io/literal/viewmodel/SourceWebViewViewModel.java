package io.literal.viewmodel;

import android.content.res.AssetManager;
import android.util.JsonReader;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.apache.commons.io.IOUtils;
import org.w3c.dom.Text;

import java.io.IOException;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

import io.literal.model.RangeSelector;
import io.literal.model.TextPositionSelector;
import io.literal.model.XPathSelector;

public class SourceWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<String> injectedGetSelectionScript = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayList<RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void>>> selectors = new MutableLiveData<>(new ArrayList<>());

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

    public void createSelector(String json) {
        JsonReader reader = new JsonReader(new StringReader(json));
        try {
            RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void> rangeSelector = RangeSelector.fromJson(reader, (rangeSelectorReader) -> {
                return XPathSelector.fromJson(rangeSelectorReader, (refinedByReader) -> {
                    return TextPositionSelector.fromJson(refinedByReader, null);
                });
            }, null);
        } catch (Exception e) {
            Log.d("SourceWebViewViewModel", "createSelector", e);
        } finally {
            try {
                reader.close();
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "createSelector", e);
            }
        }
    }
}
