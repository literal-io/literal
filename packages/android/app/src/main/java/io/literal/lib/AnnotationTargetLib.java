package io.literal.lib;

import java.util.UUID;

public class AnnotationTargetLib {
    public static String makeId(String annotationId) {
        return annotationId + "/targets/" + UUID.randomUUID().toString();
    }
}
