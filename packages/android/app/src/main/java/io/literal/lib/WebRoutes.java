package io.literal.lib;

import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;

import io.literal.BuildConfig;
import io.literal.factory.AWSMobileClientFactory;
import io.literal.repository.ErrorRepository;

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

    public static String creatorsId(String host, String identityId) {
        return host + "/creators/" + identityId;
    }

    public static String creatorsId(String identityId) {
        String encodedIdentityId = identityId;
        try {
            encodedIdentityId = URLEncoder.encode(identityId, "UTF-8");
        } catch (UnsupportedEncodingException e) {
            ErrorRepository.captureException(e);
        }
        return getWebHost() + "/creators/" + encodedIdentityId;
    }

    public static String creatorsIdAnnotationId(
            String host,
            String identityId,
            String annotationIdComponent
    ) {
        return creatorsId(host, identityId) + "/annotations/" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationsNew(String identityId) {
        return creatorsId(identityId) + "/annotations/new";
    }

    public static String creatorsIdAnnotationsNewAnnotationId(
            String identityId,
            String annotationIdComponent
    ) {
        return creatorsId(identityId) + "/annotations/new?id=" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationsNewFromMessageEvent(String identityId) {
        return creatorsIdAnnotationsNew(identityId) + "?fromMessageEvent=true";
    }

    public static String creatorsIdAnnotationId(String identityId, String annotationIdComponent) {
        return creatorsIdAnnotationId(getWebHost(), identityId, annotationIdComponent);
    }

    public static String creatorsIdAnnotationCollectionId(String host, String identityId, String annotationCollectionIdComponent) {
        return creatorsId(host, identityId) + "/annotation-collections/" + annotationCollectionIdComponent;
    }

    public static String creatorsIdAnnotationCollectionId(String identityId, String annotationCollectionIdComponent) {
        return creatorsIdAnnotationCollectionId(getWebHost(), identityId, annotationCollectionIdComponent);
    }

    public static String creatorsIdAnnotationCollectionIdAnnotationId(
            String host,
            String identityId,
            String annotationCollectionIdComponent,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationCollectionId(host, identityId, annotationCollectionIdComponent) + "?annotationId=" + annotationIdComponent;
    }

    public static String creatorsIdAnnotationCollectionIdAnnotationId(
            String identityId,
            String annotationCollectionIdComponent,
            String annotationIdComponent
    ) {
        return creatorsIdAnnotationCollectionIdAnnotationId(getWebHost(), identityId, annotationCollectionIdComponent, annotationIdComponent) + "?annotationId=" + annotationIdComponent;
    }

    public static String creatorsIdWebview(String identityId) {
        return creatorsId(identityId) + "/webview";
    }
}
