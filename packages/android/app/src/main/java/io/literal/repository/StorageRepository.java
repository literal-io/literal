package io.literal.repository;

import android.content.Context;

import com.amazonaws.mobile.client.AWSMobileClient;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferListener;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferObserver;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferState;
import com.amazonaws.mobileconnectors.s3.transferutility.TransferUtility;
import com.amazonaws.services.s3.AmazonS3URI;

import org.json.JSONObject;

import java.io.File;
import java.net.URL;
import java.util.Date;

import io.literal.factory.AWSMobileClientFactory;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.Callback;

public class StorageRepository {

    public static String getPrivatePath(String creatorIdentityId, String path) {
        return "private/" + creatorIdentityId + "/" + path;
    }

    public static String getBucket(Context context) {
        JSONObject s3TransferUtilityJson = AppSyncClientFactory
                .getConfiguration(context)
                .optJsonObject("S3TransferUtility");
        return s3TransferUtilityJson.optString("Bucket");
    }

    public static void upload(Context context, String key, File inputFile, Callback<Exception, AmazonS3URI> onUploadComplete) {
        TransferUtility transferUtility = AWSMobileClientFactory.getTransferUtility(context);

        TransferObserver transferObserver = transferUtility.upload(key, inputFile);
        transferObserver.setTransferListener(new TransferListener() {
            @Override
            public void onStateChanged(int id, TransferState state) {
                if (TransferState.COMPLETED != state) {
                    return;
                }
                AmazonS3URI s3URI = new AmazonS3URI("s3://" + transferObserver.getBucket() + "/" + transferObserver.getKey());
                onUploadComplete.invoke(null, s3URI);
            }

            @Override
            public void onProgressChanged(int id, long bytesCurrent, long bytesTotal) {
                /** noop **/
            }

            @Override
            public void onError(int id, Exception e) {
                onUploadComplete.invoke(e, null);
            }
        });
    }

    private static void download(Context context, String key, File outputFile, Callback<Exception, File> onDownloadComplete) {
        TransferObserver transferObserver = AWSMobileClientFactory.getTransferUtility(context)
                .download(key, outputFile);
        transferObserver.setTransferListener(new TransferListener() {
            @Override
            public void onStateChanged(int id, TransferState state) {
                if (TransferState.COMPLETED != state) {
                    return;
                }
                onDownloadComplete.invoke(null, outputFile);
            }

            @Override
            public void onProgressChanged(int id, long bytesCurrent, long bytesTotal) {
                /** noop **/
            }

            @Override
            public void onError(int id, Exception ex) {
                onDownloadComplete.invoke(ex, null);
            }
        });
    }
}
