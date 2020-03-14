package io.literal.ui;

import io.literal.R;
import io.literal.factory.AppSyncClientFactory;
import io.literal.lib.ContentResolverLib;

import type.CreateScreenshotInput;
import type.S3ObjectInput;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

import com.amazonaws.amplify.generated.graphql.CreateScreenshotMutation;

import com.apollographql.apollo.GraphQLCall;
import com.apollographql.apollo.api.Response;
import com.apollographql.apollo.exception.ApolloException;

import org.json.JSONObject;

import java.io.File;
import java.util.UUID;

import javax.annotation.Nonnull;

public class ShareTargetHandler extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_share_target_handler);

        Intent intent = getIntent();
        String action = intent.getAction();
        String type = intent.getType();

        if (Intent.ACTION_SEND.equals(action) && type != null && type.startsWith("image/")) {
            handleSendImage(intent);
        } else {
            handleSendNotSupported();
        }
    }

    void handleSendImage(Intent intent) {

        JSONObject s3TransferUtilityJson = AppSyncClientFactory
                .getConfiguration(this)
                .optJsonObject("S3TransferUtility");
        String bucket = s3TransferUtilityJson.optString("Bucket");
        String region = s3TransferUtilityJson.optString("Region");

        String screenshotId = UUID.randomUUID().toString();
        Uri imageUri = intent.getParcelableExtra(Intent.EXTRA_STREAM);
        String filePath = "screenshots/" + id;
        File file = ContentResolverLib.toFile(this, imageUri, filePath);

        CreateScreenshotMutation createScreenshotMutation = CreateScreenshotMutation
                .builder()
                .input(
                        CreateScreenshotInput.builder()
                                .id(screenshotId)
                                .file(
                                        S3ObjectInput.builder()
                                                .bucket(bucket)
                                                .key("public/screenshot/" + screenshotId)
                                                .region(region)
                                                .localUri(file.getAbsolutePath())
                                                .mimeType(intent.getType())
                                                .build()
                                )
                                .build()
                )
                .build();
        AppSyncClientFactory.getInstance(this).mutate(createScreenshotMutation).enqueue(screenshotsCallback);
    }

    private GraphQLCall.Callback<CreateScreenshotMutation.Data> screenshotsCallback = new GraphQLCall.Callback<CreateScreenshotMutation.Data>() {
        @Override
        public void onResponse(@Nonnull Response<CreateScreenshotMutation.Data> response) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(ShareTargetHandler.this, "Added Highlight", Toast.LENGTH_SHORT);
                    ShareTargetHandler.this.finish();
                }
            });
        }

        @Override
        public void onFailure(@Nonnull final ApolloException e) {
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.e("", "Failed to perform CreateScreenshotMutation", e);
                    Toast.makeText(ShareTargetHandler.this, "Failed to add highlight", Toast.LENGTH_SHORT);
                    ShareTargetHandler.this.finish();
                }
            });
        }
    };

    void handleSendNotSupported() {
        // TODO: implement fallback handling, e.g. display a "This does not look like a screenshot" UI
    }
}
