package io.literal.lib;

import io.literal.repository.ErrorRepository;

public class AnnotationCollectionLib {
    public static String makeId(String creatorUsername, String labelText) {
        try {
            String idComponent = Crypto.sha256Hex(labelText);
            return WebRoutes.creatorsIdAnnotationCollectionId(
                    WebRoutes.getAPIHost(),
                    creatorUsername,
                    idComponent
            );
        } catch (java.security.NoSuchAlgorithmException ex) {
            ErrorRepository.captureException(ex);
            return null;
        }
    }
}
