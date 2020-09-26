package io.literal.lib;

public class AnnotationLib {
    public static String idComponentFromId(String annotationId) {
        String[] parts = annotationId.split("/");
        return parts[parts.length - 1];
    }
}
