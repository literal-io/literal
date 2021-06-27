package io.literal.lib;

import android.content.Context;

import com.amazonaws.services.s3.AmazonS3URI;

import java.net.URI;
import java.net.URISyntaxException;

import io.literal.repository.StorageRepository;

public class AmazonS3URILib {

    /**
     * AmazonS3URI.getURI formats with s3:// scheme, we persist https:// equivalent for consistency
     */
    public static URI toHTTPs(Context context, AmazonS3URI uri) {
        try {
            return new URI(
                    "https",
                    uri.getURI().getHost() + ".s3." + StorageRepository.getBucketRegion(context) +".amazonaws.com",
                    uri.getURI().getPath(),
                    null
            );
        } catch (URISyntaxException e) {
            return null;
        }
    }
}
