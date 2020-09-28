package io.literal.lib;

import io.literal.BuildConfig;
import io.literal.factory.AWSMobileClientFactory;

public class WebRoutes {

    public static String getAPIHost() {
        if (
                AWSMobileClientFactory.getAmplifyEnvironment() ==
                        AWSMobileClientFactory.AmplifyEnvironment.PRODUCTION
        ) {
            return "https://literal.io";
        } else {
            return "https://staging.literal.io";
        }
    }

    public static String getWebHost() {
        if (BuildConfig.DEBUG) {
            return "http://localhost:3000";
        } else {
            return getAPIHost();
        }
    }

    public static String authenticate() {
        return getWebHost() + "/authenticate";
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
        return getWebHost() + "/creators/" + creatorUsername + "/annotations/new?id=" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationId(
            String creatorUsername,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationId(getWebHost(), creatorUsername, annotationIdComponent);
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
        return creatorsIdAnnotationCollectionId(getWebHost(), creatorUsername, annotationCollectionIdComponent);
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
        return creatorsIdAnnotationCollectionIdAnnotationId(getWebHost(), creatorUsername, annotationCollectionIdComponent, annotationIdComponent) + "?annotationId=" + annotationIdComponent;
    }
}
