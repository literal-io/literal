package io.literal.repository;

import android.content.Context;
import android.net.Uri;
import android.webkit.MimeTypeMap;

import com.amazonaws.services.s3.AmazonS3URI;
import com.amazonaws.services.s3.model.ObjectMetadata;

import java.io.File;
import java.net.URL;

import io.literal.lib.Callback;

public class ArchiveRepository {
    private static String storageHost;

    public static File getLocalDir(Context context) {
        File archiveDir = new File(context.getCacheDir(), "archives");
        if (!archiveDir.exists()) {
            archiveDir.mkdirs();
        }

        return archiveDir;
    }

    public static void upload(Context context, File archive, String creatorIdentityId, Callback<Exception, AmazonS3URI> onUploadComplete) {
        ObjectMetadata metadata = new ObjectMetadata();
        metadata.setCacheControl("public, max-age=604800, immutable");
        metadata.setContentType("application/x-mimearchive");
        String id = archive.getName().substring(0, archive.getName().indexOf("."));

        StorageRepository.upload(
                context,
                StorageRepository.getPrivatePath(creatorIdentityId, "archives/" + id + "/archive.mhtml"),
                archive,
                metadata,
                onUploadComplete
        );
    }

    public static void download(Context context, AmazonS3URI uri, Callback<Exception, File> onDownload) {
        String key = uri.getKey();
        String basename = key.substring(key.lastIndexOf("/") + 1);
        File cacheFile = new File(getLocalDir(context), basename);

        if (cacheFile.exists()) {
            onDownload.invoke(null, cacheFile);
            return;
        }

        StorageRepository.download(
                context,
                uri.getKey(),
                cacheFile,
                onDownload
        );
    }
}
