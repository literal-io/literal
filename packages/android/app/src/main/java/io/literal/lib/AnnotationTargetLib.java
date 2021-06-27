package io.literal.lib;

import java.util.UUID;

import io.literal.model.ExternalTarget;
import io.literal.model.SpecificTarget;
import io.literal.model.Target;
import io.literal.model.TextualTarget;

public class AnnotationTargetLib {
    public static String makeId(String annotationId) {
        return annotationId + "/targets/" + UUID.randomUUID().toString();
    }

    public static String getID(Target target) {
        if (target instanceof ExternalTarget) {
            return ((ExternalTarget) target).getId().toString();
        } else if (target instanceof SpecificTarget) {
            return ((SpecificTarget) target).getId();
        } else {
            return ((TextualTarget) target).getId();
        }
    };
}
