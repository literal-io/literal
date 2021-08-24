package io.literal.model;

import android.content.Context;
import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;
import android.util.Log;
import android.webkit.WebResourceRequest;

import org.apache.commons.io.IOUtils;
import org.apache.james.mime4j.Charsets;
import org.apache.james.mime4j.dom.Entity;
import org.apache.james.mime4j.dom.Message;
import org.apache.james.mime4j.dom.Multipart;
import org.apache.james.mime4j.dom.SingleBody;
import org.apache.james.mime4j.dom.field.ContentIdField;
import org.apache.james.mime4j.dom.field.ContentLocationField;
import org.apache.james.mime4j.message.BodyPart;
import org.apache.james.mime4j.message.BodyPartBuilder;
import org.apache.james.mime4j.message.DefaultMessageBuilder;
import org.apache.james.mime4j.message.DefaultMessageWriter;
import org.apache.james.mime4j.stream.FieldBuilder;
import org.apache.james.mime4j.stream.MimeConfig;
import org.apache.james.mime4j.stream.RawField;
import org.jetbrains.annotations.NotNull;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;
import java.util.function.Supplier;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.stream.Collectors;

import io.literal.repository.ErrorRepository;
import io.literal.repository.WebArchiveRepository;

public class WebArchive {
    public static Pattern CONTENT_ID_FIELD_PATTERN = Pattern.compile("<frame-(.+)>");
    private static String FALLBACK_INDEX_FIELD_NAME = "Literal-Fallback-Index-Document";
    private static String FALLBACK_INDEX_FIELD_VALUE = "Literal-Fallback-Index-Document";

    private final StorageObject storageObject;
    private final ArrayList<WebResourceRequest> webRequests;
    private final ArrayList<HTMLScriptElement> scriptElements;
    private String id;
    private Message mimeMessage;
    private HashMap<String, BodyPart> bodyPartByContentLocation;
    private HashMap<String, BodyPart> bodyPartByContentID;
    private CompletableFuture<Void> openCompletableFuture;

    public WebArchive(
            @NotNull StorageObject storageObject,
            @NotNull ArrayList<WebResourceRequest> webRequests,
            @NotNull ArrayList<HTMLScriptElement> scriptElements
    ) {
        this.storageObject = storageObject;
        this.webRequests = webRequests;
        this.scriptElements = scriptElements;
        this.mimeMessage = null;
    }

    public WebArchive(@NotNull StorageObject storageObject) {
        this.storageObject = storageObject;
        this.webRequests = new ArrayList<>();
        this.scriptElements = new ArrayList<>();
        this.mimeMessage = null;
    }

    public StorageObject getStorageObject() {
        return storageObject;
    }

    public List<WebResourceRequest> getWebRequests() {
        return webRequests;
    }

    public List<HTMLScriptElement> getScriptElements() {
        return scriptElements;
    }

    public CompletableFuture<Void> open(Context context, User user) {
        if (mimeMessage != null) {
            return CompletableFuture.completedFuture(null);
        }

        if (this.openCompletableFuture != null) {
            return this.openCompletableFuture;
        }

        this.openCompletableFuture = storageObject.download(context, user)
                .thenCompose((_void) -> {
                    CompletableFuture<Void> future = new CompletableFuture<>();
                    FileInputStream fileInputStream = null;
                    try {
                        fileInputStream = new FileInputStream(storageObject.getFile(context));
                        DefaultMessageBuilder messageBuilder = new DefaultMessageBuilder();
                        messageBuilder.setMimeEntityConfig(new MimeConfig.Builder().setMaxLineLen(-1).setMaxHeaderLen(-1).setMaxHeaderCount(-1).build());
                        mimeMessage = messageBuilder.parseMessage(fileInputStream);
                        buildMimeBodyPartIndex(context, user);
                        future.complete(null);
                    } catch (Exception innerException) {
                        future.completeExceptionally(innerException);
                    } finally {
                        if (fileInputStream != null) {
                            try {
                                fileInputStream.close();
                            } catch (IOException ioException) {
                                ErrorRepository.captureException(ioException);
                            }
                        }
                    }
                    return future;
                });

        return this.openCompletableFuture;
    }

    public void dispose() {
        if (mimeMessage != null) {
            this.bodyPartByContentLocation.clear();
            this.bodyPartByContentID.clear();
            this.mimeMessage.dispose();
            this.mimeMessage = null;
        }
    }

    private List<Entity> getBodyParts() {
        if (mimeMessage == null) {
            ErrorRepository.captureException(new Exception("Invalid state: attempted to build mime body part index, but mimeMessage is null."));
            return null;
        }

        Multipart multipart = (Multipart) mimeMessage.getBody();
        return multipart.getBodyParts();
    }

    private void buildMimeBodyPartIndex(Context context, User user) {
        List<Entity> bodyParts = this.getBodyParts();

        bodyPartByContentLocation = bodyParts.stream().collect(
                HashMap::new,
                (agg, bodyPart) -> {
                    ContentLocationField contentLocation = (ContentLocationField) bodyPart.getHeader().getField("Content-Location");

                    // Not all body parts will have a Content-Location header, e.g. subframes are keyed on Content-ID instead.
                    if (contentLocation != null) {
                        agg.put(contentLocation.getLocation(), (BodyPart) bodyPart);
                    }
                },
                HashMap::putAll
        );

        bodyPartByContentID = bodyParts.stream().collect(
                HashMap::new,
                (agg, bodyPart) -> {
                    ContentIdField contentID = (ContentIdField) bodyPart.getHeader().getField("Content-ID");

                    if (contentID != null && contentID.getId() != null) {
                        Matcher contentIDMatcher = CONTENT_ID_FIELD_PATTERN.matcher(contentID.getId());
                        if (contentIDMatcher.find()) {
                            String parsedContentID = "cid:frame-" + contentIDMatcher.group(1);
                            agg.put(parsedContentID, (BodyPart) bodyPart);
                        } else {
                            // Bare CIDs exist, e.g. "cid:css-"
                            agg.put(contentID.getId(), (BodyPart) bodyPart);
                        }
                    }
                },
                HashMap::putAll
        );

        // Alias the canonical archive URI with the primary index document.
        bodyPartByContentLocation.put(
                storageObject.getAmazonS3URI(context, user).toString(),
                (BodyPart) bodyParts.get(0)
        );
    }

    public CompletableFuture<WebArchive> compile(Context context, User user) {
        return this.open(context, user)
                .thenCompose(_file -> {
                    // For each web request not currently within the archive, execute it and build a BodyPart
                    HashMap<String, BodyPart> bodyPartByContentLocation = getBodyPartByContentLocation();
                    List<CompletableFuture<Optional<BodyPart>>> webRequestBodyPartFutures = getWebRequests()
                            .stream()
                            .filter(webResourceRequest -> !bodyPartByContentLocation.containsKey(webResourceRequest.getUrl().toString()))
                            .map(webResourceRequest ->
                                    WebArchiveRepository.executeWebResourceRequest(webResourceRequest)
                                            .thenApply((responseBody) -> WebArchiveRepository.createBodyPart(webResourceRequest, responseBody))
                                            .<Optional<BodyPart>>handle((bodyPart, e) -> {
                                                // Tolerate any errors that occurred when replaying the web request - the asset we attempted to fetch may not be required.
                                                if (e != null) {
                                                    ErrorRepository.captureException(e, Map.of("headers", WebArchiveRepository.getIdempotentRequestHeaders(webResourceRequest.getRequestHeaders())));
                                                    return Optional.empty();
                                                }
                                                return bodyPart;
                                            })
                            )
                            .collect(Collectors.toList());
                    return CompletableFuture.allOf(webRequestBodyPartFutures.toArray(new CompletableFuture[0]))
                            .thenApply(_void -> webRequestBodyPartFutures.stream()
                                    .map((f) -> f.getNow(Optional.empty()).orElse(null))
                                    .filter(Objects::nonNull)
                                    .collect(Collectors.toList()));
                })
                .thenCompose((bodyParts) -> {
                    // Add the constructed web request body parts into the mime message, and update the primary
                    // index body part with built script elements.
                    Multipart multipart = (Multipart) mimeMessage.getBody();
                    bodyParts.forEach(multipart::addBodyPart);
                    BodyPart updatedIndexBodyPart = addScriptsToBodyPart((BodyPart) multipart.getBodyParts().get(0), getScriptElements());
                    multipart.addBodyPart(updatedIndexBodyPart, 0);

                    // Update the old primary index body part with a header to identify it, in certain instances
                    // we'll fallback to this document.
                    BodyPart updatedOriginalIndexBodyPart = addOriginalIndexDocumentHeaderToBodyPart((BodyPart) multipart.getBodyParts().get(1));
                   // multipart.replaceBodyPart(updatedOriginalIndexBodyPart, 0);

                    // Write the updated mime message to disk
                    StorageObject updatedWebArchive = WebArchiveRepository.createArchiveStorageObject();
                    FileOutputStream updatedWebArchiveOutputStream = null;
                    CompletableFuture<WebArchive> future = new CompletableFuture<>();
                    try {
                        updatedWebArchiveOutputStream = new FileOutputStream(updatedWebArchive.getFile(context));
                        DefaultMessageWriter writer = new DefaultMessageWriter();
                        writer.writeMessage(mimeMessage, updatedWebArchiveOutputStream);
                        updatedWebArchive.setStatus(StorageObject.Status.UPLOAD_REQUIRED);

                        future.complete(new WebArchive(updatedWebArchive));
                    } catch (Exception e) {
                        future.completeExceptionally(e);
                    } finally {
                        if (updatedWebArchiveOutputStream != null) {
                            try {
                                updatedWebArchiveOutputStream.close();
                            } catch (Exception e1) {
                                ErrorRepository.captureException(e1);
                            }
                        }
                    }
                    return future;
                });
    }

    private BodyPart addOriginalIndexDocumentHeaderToBodyPart(BodyPart bodyPart) {
        bodyPart.getHeader().addField(new RawField(FALLBACK_INDEX_FIELD_NAME, FALLBACK_INDEX_FIELD_VALUE));
        return bodyPart;
    }

    private BodyPart addScriptsToBodyPart(BodyPart bodyPart, List<HTMLScriptElement> scriptElements) {
        try {
            SingleBody body = (SingleBody) bodyPart.getBody();
            String html = IOUtils.toString(body.getInputStream(), StandardCharsets.UTF_8);
            int closeHeadElemIdx = html.indexOf("</body>");
            if (closeHeadElemIdx == -1) {
                ErrorRepository.captureException(new Exception("Unable to locate index of head element within archive."));
                return bodyPart;
            }

            int stringBuilderInitialCapacity = scriptElements.stream().map(s -> s.getText().length()).reduce(0, Integer::sum) + (scriptElements.size() * 24);
            StringBuilder scriptsStringBuilder = new StringBuilder(stringBuilderInitialCapacity);
            scriptElements.forEach((scriptElement -> {
                scriptElement.appendToStringBuilder(scriptsStringBuilder);
                scriptsStringBuilder.append("\n");
            }));
            StringBuilder htmlStringBuilder = new StringBuilder(html);
            htmlStringBuilder.insert(closeHeadElemIdx, scriptsStringBuilder.toString());

            BodyPartBuilder bodyPartBuilder = BodyPartBuilder.create()
                    .setBody(htmlStringBuilder.toString(), Charsets.UTF_8)
                    .setContentDisposition(bodyPart.getDispositionType())
                    .setContentTransferEncoding(bodyPart.getContentTransferEncoding())
                    .setContentType(bodyPart.getMimeType());
            bodyPart.getHeader().getFields().forEach(bodyPartBuilder::setField);

            return bodyPartBuilder.build();
        } catch (Exception e) {
            ErrorRepository.captureException(e);
            return bodyPart;
        }
    }

    public Optional<BodyPart> resolveWebResourceRequest(WebResourceRequest request, boolean isJavaScriptEnabled) {
        Optional<BodyPart> optionalBodyPart = Optional.empty();

        // If JavaScript is disabled, attempt to find the original index document using a custom header added
        // during archive creation.
        if (request.isForMainFrame() && !isJavaScriptEnabled) {
            optionalBodyPart = Optional.ofNullable(this.getBodyParts())
                    .flatMap((bodyParts) -> bodyParts.stream()
                            .filter(bodyPart ->
                                    bodyPart.getHeader().getFields().stream().anyMatch((field -> field.getName().equals(FALLBACK_INDEX_FIELD_NAME)))
                            )
                            .findFirst()
                            .map((entity) -> (BodyPart) entity)
                    );
        }


        // Sub-documents appear with the same Content-Location as the main document, so pull the first
        // body part from the archive to ensure we receive the main document content.
        if (request.isForMainFrame() && isJavaScriptEnabled) {
            optionalBodyPart = Optional.ofNullable(this.getBodyParts()).flatMap(bodyParts -> Optional.ofNullable((BodyPart) bodyParts.get(0)));
        }

        // Chrome appears to use CID URIs within Content-Location sometimes (e.g. "cid:css-"), fallback to checking
        // primary content location index after checking Content-ID index.
        if (!optionalBodyPart.isPresent() && request.getUrl().getScheme().equals("cid")) {
            optionalBodyPart = Optional.ofNullable(bodyPartByContentID.get(request.getUrl().toString()));
        }

        // Fallback to attempting to resolve by Content-Location.
        if (!optionalBodyPart.isPresent()) {
            optionalBodyPart = Optional.ofNullable(bodyPartByContentLocation.get(request.getUrl().toString()));
        }

        return optionalBodyPart;
    }

    public HashMap<String, BodyPart> getBodyPartByContentLocation() {
        return bodyPartByContentLocation;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public HashMap<String, BodyPart> getBodyPartByContentID() {
        return bodyPartByContentID;
    }
}
