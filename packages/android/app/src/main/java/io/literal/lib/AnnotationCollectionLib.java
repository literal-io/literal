package io.literal.lib;

public class AnnotationCollectionLib {
    public static String makeId(String creatorUsername, String labelText) {
        try {
            String idComponent = Crypto.sha256Hex(labelText);
            return WebRoutes.creatorsIdAnnotationCollectionId(
                    Constants.API_HOST,
                    creatorUsername,
                    idComponent
            );
        } catch (java.security.NoSuchAlgorithmException ex) {
            return null;
        }
    }
}
