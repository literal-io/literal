package io.literal.model;

public enum Format {
    APPLICATION_X_MIMEARCHIVE,
    APPLICATION_OCTET_STREAM,
    IMAGE_PNG,
    TEXT_PLAIN,
    TEXT_HTML;

    public type.Format toGraphQL() {
        return type.Format.valueOf(this.name());
    }

    public static Format fromMimeType(String mimeType) {
        switch (mimeType.toLowerCase()) {
            case "image/png": return IMAGE_PNG;
            case "application/x-mimearchive": return APPLICATION_X_MIMEARCHIVE;
            case "text/plain": return TEXT_PLAIN;
            case "text/html": return TEXT_HTML;
            default: return APPLICATION_OCTET_STREAM;
        }
    }

    public String toMimeType() {
        switch (this) {
            case APPLICATION_X_MIMEARCHIVE: return "application/x-mimearchive";
            case IMAGE_PNG: return "image/png";
            case TEXT_PLAIN: return "text/plain";
            case TEXT_HTML: return "text/html";
            default: return "application/octet-stream";
        }
    }
}
