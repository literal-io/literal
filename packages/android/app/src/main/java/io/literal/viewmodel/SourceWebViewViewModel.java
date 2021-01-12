package io.literal.viewmodel;

import android.content.res.AssetManager;
import android.util.JsonReader;
import android.util.Log;

import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.io.StringReader;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

import io.literal.model.RangeSelector;
import io.literal.model.TextPositionSelector;
import io.literal.model.XPathSelector;

public class SourceWebViewViewModel extends ViewModel {
    private final MutableLiveData<Boolean> hasFinishedInitializing = new MutableLiveData<>(false);
    private final MutableLiveData<String> getSelectorScript = new MutableLiveData<>(null);
    private final MutableLiveData<String> highlightSelectorScript = new MutableLiveData<>(null);
    private final MutableLiveData<ArrayList<RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void>>> selectors = new MutableLiveData<>(new ArrayList<>());

    private static final String GET_SELECTOR_SCRIPT_NAME = "SourceWebViewGetSelector.js";
    private static final String HIGHLIGHT_SELECTOR_SCRIPT_NAME = "SourceWebViewHighlightSelectors.js";

    public MutableLiveData<Boolean> getHasFinishedInitializing() {
        return hasFinishedInitializing;
    }

    public void setHasFinishedInitializing(Boolean hasFinishedInitializing) {
        this.hasFinishedInitializing.setValue(hasFinishedInitializing);
    }

    public String getGetSelectorScript(AssetManager assetManager) {
        if (getSelectorScript.getValue() == null) {
            try {
                getSelectorScript.setValue(
                        IOUtils.toString(assetManager.open(GET_SELECTOR_SCRIPT_NAME), StandardCharsets.UTF_8)
                );
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "getGetSelectorScript", e);
            }
        }
        return getSelectorScript.getValue();
    }

    public String getHighlightSelectorScript(AssetManager assetManager, JSONArray paramSelectors) {
        if (highlightSelectorScript.getValue() == null) {
            try {
                highlightSelectorScript.setValue(
                        IOUtils.toString(assetManager.open(HIGHLIGHT_SELECTOR_SCRIPT_NAME), StandardCharsets.UTF_8)
                );
            } catch (IOException e) {
                Log.d("SourceWebViewViewModel", "getHighlightSelectorScript", e);
            }
        }

        String script = highlightSelectorScript.getValue().replaceAll("\\$\\{PARAM_SELECTORS\\}", paramSelectors.toString());

        Log.d("SourceWebViewViewModel", "script: " + script);
        return script;
    }

    public MutableLiveData<ArrayList<RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void>>> getSelectors() {
        return selectors;
    }

    public void createSelector(String json) {
        JsonReader reader = new JsonReader(new StringReader(json));
        try {
            RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void> selector = RangeSelector.fromJson(reader, (rangeSelectorReader) -> {
                return XPathSelector.fromJson(rangeSelectorReader, (refinedByReader) -> {
                    return TextPositionSelector.fromJson(refinedByReader, null);
                });
            }, null);

            ArrayList<RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void>> newSelectors = (ArrayList<RangeSelector<XPathSelector<TextPositionSelector<Void>>, Void>>) selectors.getValue().clone();
            newSelectors.add(selector);
            selectors.setValue(newSelectors);
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
