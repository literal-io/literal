package io.literal.lib;

public class WebRoutes {
    public static String authentication() {
        return Constants.WEB_HOST + "/authentication";
    }

    public static String creatorsIdAnnotationId(
            String host,
            String creatorUsername,
            String annotationIdComponent
    ) {
        return host + "/creators/" + creatorUsername + "/annotations/" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationsNewAnnotationId(
            String creatorUsername,
            String annotationIdComponent
    ) {
        return Constants.WEB_HOST + "/creators/" + creatorUsername + "/annotations/new?id=" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationId(
            String creatorUsername,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationId(Constants.WEB_HOST, creatorUsername, annotationIdComponent);
    }

    public static String creatorsIdAnnotationCollectionId(
            String host,
            String creatorUsername,
            String annotationCollectionIdComponent
    ) {
        return host + "/creators/" + creatorUsername + "/annotation-collections/" + annotationCollectionIdComponent;
    }

    public static String creatorsIdAnnotationCollectionId(
            String creatorUsername,
            String annotationCollectionIdComponent
    ) {
        return creatorsIdAnnotationCollectionId(Constants.WEB_HOST, creatorUsername, annotationCollectionIdComponent);
    }

    public static String creatorsIdAnnotationCollectionIdAnnotationId(
            String host,
            String creatorUsername,
            String annotationCollectionIdComponent,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationCollectionId(host, creatorUsername, annotationCollectionIdComponent) + "?annotationId=" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationCollectionIdAnnotationId(
            String creatorUsername,
            String annotationCollectionIdComponent,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationCollectionIdAnnotationId(Constants.WEB_HOST, creatorUsername, annotationCollectionIdComponent, annotationIdComponent) + "?annotationId=" + annotationIdComponent;
    }
}
