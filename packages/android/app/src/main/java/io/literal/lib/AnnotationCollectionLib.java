package io.literal.lib;

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
            return null;
        }
    }
}
