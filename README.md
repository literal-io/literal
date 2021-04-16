# Literal

Literal is a textual annotation management system. Capture ideas and phrases from text that you read. Build a digital garden of words.

## How It Works

To add an annotation to Literal:

1. **Highlight Text.** Literal supports annotations made within web browsers, PDF readers, and more.
2. **Import the Highlight.** Share the highlight with Literal to import it. For content types that Literal supports natively (e.g. Web), share the content to import and archive it, and annotate within the Source Viewer. For all other content types, use the selection toolbar to share the highlighted text directly, or share a screenshot of the highlight.
3. **Annotate the Highlight.** Add context and thoughts by adding tags and notes.
4. **Explore.** Browse annotations grouped by tags and reflect as ideas evolve in response to new annotations and new relationships between them.

## Demo Videos

- [How to annotate the Web](https://www.youtube.com/watch?v=nH1ukQY3Ia8)
- [How to annotate Twitter](https://www.youtube.com/watch?v=s7hps6_4VTU)
- [How to annotate PDF](https://www.youtube.com/watch?v=9NurlekUeZ8)

## Status

Literal is actively being used, but is also under active development. If you have ideas for features or encounter a bug, please open an issue.

Literal is distributed as an Android application, with support for other platforms planned. To indicate your support for a platform add a reaction or comment on the issue for [adding iOS support](https://github.com/literal-io/literal/issues/81), [adding Web support](https://github.com/literal-io/literal/issues/82), [adding Web Extension support](https://github.com/literal-io/literal/issues/83), or by creating an issue for adding support to your platform of choice.

Releases are regularly published on [Github](https://github.com/literal-io/literal/releases) and [Google Play](https://play.google.com/store/apps/details?id=io.literal).

## Architecture

Literal is a client for the [W3C Web Annotation Data Model](https://www.w3.org/TR/annotation-model/), and uses this model for all data storage and transmission. A closed-source GraphQL API implementing this data model is used for data persistance, though in the future alternate APIs (including local-only APIs) may be supported. There are some application specific extensions to the model that are annotated appropriately, but the intent is to hew as close as possible to the original specification.

## Precepts

### Separate signal from noise.

Interacting with text elevates the reading experience beyond one of passive consumption and into one that requires analysis and critical thought. Annotations are of equal value to the text itself.

### Build bridges, not silos.

Ideas do not exist in isolation. Ideas are threads that span otherwise disparate contexts. Annotations organized strictly by their original source is an artificial limitation.

### Understand the source.

Annotations removed from their original context are like a verse without a song, or a headline without an article. Given a highlight, you should frequently reexamine it within its original context.

### Open by default.

Information flows like ripples in a pond, and cascades to create and amplify network effects. Data - annotations and sources, creators and consumers - should be free to move across the boundaries of systems.
