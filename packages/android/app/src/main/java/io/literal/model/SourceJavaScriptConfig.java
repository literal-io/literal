package io.literal.model;

public class SourceJavaScriptConfig {
    public enum Reason {
        AUTOMATIC,
        USER_TOGGLED
    }
    private final boolean isEnabled;
    private final Reason reason;

    public SourceJavaScriptConfig(boolean isEnabled, Reason reason) {
        this.isEnabled = isEnabled;
        this.reason = reason;
    }

    public boolean isEnabled() {
        return isEnabled;
    }

    public Reason getReason() {
        return reason;
    }
}
