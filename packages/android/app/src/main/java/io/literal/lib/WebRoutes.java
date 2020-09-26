package io.literal.lib;

public class WebRoutes {
    public static String authentication() {
        return Constants.WEB_HOST + "/authentication";
    }

    public static String creatorsIdAnnotationCollectionId(
            String creatorUsername,
            String annotationCollectionIdComponent
    ) {
        return Constants.WEB_HOST + "/creators/" + creatorUsername + "/annotation-collections/" + annotationCollectionIdComponent;
    }

    public static String creatorsIdAnnotationCollectionIdAnnotationId(
            String creatorUsername,
            String annotationCollectionIdComponent,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationCollectionId(creatorUsername, annotationCollectionIdComponent) + "?annotationId=" + annotationIdComponent;
    }
}
