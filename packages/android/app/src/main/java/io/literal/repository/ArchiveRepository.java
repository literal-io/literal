package io.literal.repository;

import android.content.Context;

import com.amazonaws.services.s3.AmazonS3URI;

import java.io.File;

import io.literal.lib.Callback;

public class ArchiveRepository {

    public static File getLocalDir(Context context) {
        return new File(context.getCacheDir(), "archives");
    }

    public static void upload(Context context, File archive, String creatorIdentityId, Callback<Exception, AmazonS3URI> onUploadComplete) {
        StorageRepository.upload(
                context,
                StorageRepository.getPrivatePath(creatorIdentityId, "archives/" + archive.getName()),
                archive,
                onUploadComplete
        );
    }
}
