Literal is now [available on the Google Play Store](https://play.google.com/store/apps/details?id=io.literal).

Literal is still very much a work in progress — there is still much work to be done. What follows should be read as a position piece or manifesto as much as a typical product announcement or walk through.

The vision for the product is as follows:

> For readers who interact with digital text, Literal is an annotation management system that enhances your reading experience. Unlike traditional digital highlight and note-taking applications, Literal enables the capturing of annotations wherever you read, retains the original context of the annotation, and enables building connections between disparate ideas and sources.

In the spirit of textual annotation, I'd like to essentially annotate this statement in sections in order to expand on several points encapsulated within it to establish the set of beliefs that I'm working from, and encompass what Literal is (and in some cases, what it _will be)_ and what it is not.

## Diagrams

From the top.

> For readers who interact with digital text, ...

This is a more limited and specific set of people than you might imagine at first glance. It is much smaller than "the set of all literate people" and still smaller than "the set of people who read online". There's been various pieces written about this general phenomenon recently (e.g. [The Erosion of Deep Literacy](https://www.nationalaffairs.com/publications/detail/the-erosion-of-deep-literacy)) such that I'll only summarize here: many people _read,_ but fewer and fewer people _deep read_, such that they are actively interacting with text as opposed to just passively consuming it.

It may seem counter-intuitive to spend time in what is effectively a product announcement outlining how small your product's addressable market is, but I'd argue that this is just a re-phrasing of the opportunity. For the most part, we're reading text online in the same way that we're read text on paper and paper analogues for thousands of years. Even today, with digital text far more accessible than alternatives, there is a vocal contingent of readers who only purchase physical books. Consider that this is not at all the case within the domain of writing — few can rationalize composing text without modern word processor functionality or the ability of being able to instantly delete or revise what was previously written, now largely taken for granted. Simply put, I think there is much work yet to be done within the realm of how we read digital text, and once done, the size of the set of readers who interact with digital text will increase considerably.

> Literal is an annotation management system...

Let's unpack the buzzword [1] and talk about specific functionality for a second. An "annotation management system" is a means for the capturing of annotations of digital text, where "annotation" is a highlight with optional text associated with it, as well as a method of storage, organization, search, and enabling the general utilization of those annotations.

Today, Literal fulfills mostly just the "capture" aspect of this, but has some simple organization functionality, and of course is capable of storage.

> that enhances your reading experience.

Moving on from specific functionality and onto the bigger picture of what that functionality enables, the intention of Literal is to improve both the process and outcome of reading. This happens in a number of ways:

1. Interacting with text via annotation elevates the reading experience beyond one of passive consumption and into one that requires analysis and critical thought. Inserting oneself into the text and creating a feedback loop of new information and existing experience via annotation consolidates and increases retention of that new information
2. Annotation is a means of distillation of an idea or concept. An annotation is a building block that can transported, rearranged, reconstructed, where as the source text is often a much larger and immutable construct. Abstraction through annotation enables the movement of ideas across disparate source contexts, allowing for ideas of wider complexity and deeper meaning to be developed.
3. Annotations serve as guideposts for the future traversal of the current experience of the source, either by your own self, or by someone else. This preservation of the current perspective allows for one to approach the source in an augmented fashion — a dialectical interaction between self, source, and third perspective. [2]

> Unlike traditional digital highlight and note-taking applications, ....

Existing software in this category includes standalone applications like [hypothesis](https://web.hypothes.is/) and [Readwise](https://readwise.io/), as well as applications that are vertically integrated with a content reader like [Kindle highlights](https://www.amazon.com/b/?node=11627044011) and iBooks, and a wide array of other PDF and EPUB applications.

These existing applications are diverse, each with their own strengths and weaknesses. Taken broadly, Literal distinguishes itself in the several ways, enumerated in the sections that follow.

Additionally, In an attempt to elucidate something by outlining its negative space: Literal is not a general purpose note taking or note management application. Applications like [Notion](https://www.notion.so/), [Roam](https://roamresearch.com/), [Evernote](https://evernote.com/), and others are designed to handle thought (writing, research, recall) in general. While this is a lofty and admirable goal, Literal will explicitly not support such open ended use cases. Textual annotation management could be thought of as a strict subset of general purpose note taking. You're still working with "thought", but "thought" limited to a particular medium with specific constraints. While you _could_ do textual annotation management with a note-taking application, an application optimized for the problem space has the opportunity to perform much better.

> Literal enables the capturing of annotations wherever you read, ...

I think the vast majority of information consumption happens on mobile devices, reading included. Deep-reading often times takes place on the web (e.g. articles and blogs) but also within external applications like PDF readers and Kindle devices.

Literal is currently mobile-only, and will remain mobile-first. Literal will have a desktop web interface, but the user experience will be primarily geared to support organization and management tasks.

Literal supports the parsing of highlights from screenshots and will extract the highlighted text. This enables the import of annotations created in most applications - web browsers, PDF readers, Kindle, and more. In the near term, I intend to add support for platform specific integrations like browser extensions.

> retains the original context of the annotation, ...

An annotation without its source is a headline without an article — only a component of a more complete picture. Existing software often ignores the source context of an annotation entirely.

What source context is, and how to capture it, of course depends on the source. In the near term, Literal will utilize the [W3C Web Annotation Data Model](https://www.w3.org/TR/annotation-model/) and content-type specific interfaces to enable source context capture. The data model is flexible and will be extended to beyond the web, e.g. PDFs, Kindle and other document formats. In the application itself, the idea is to utilize the captured context as a first-class object in the annotation ontology, thereby enabling organization and management.

> and enables building connections between disparate ideas and sources.

Capturing annotations is a means, not an end. Existing software often is full of functionality to create annotations, but much less capable of organizing or leveraging the annotations once created. Additionally, when the source context of an annotation _is_ preserved, it is often coupled to a particular source and presented directly alongside it.

Today, Literal supports the tagging of annotations, which enables the creation of source-independent collections of annotations. This is admittedly a minimal form of organization, and I'd like to continue to explore ideas on this front as the product matures.

## Closing

Ideally this has given the reader an understanding of what Literal is today, and what I intend for it to become.

Install the Android application from the Play Store [here](https://play.google.com/store/apps/details?id=io.literal).

Reach out to daniel@literal.io with thoughts, feedback, or just to say hello.

<details>
  <summary>Notes</summary>
  <ol>
    <li><p>I think this is mostly a buzzword I just invented. Most of the references to the phrase I can find are in reference to data labeling within relational database management systems</p></li>
    <li><p><a href="https://www.gwern.net/About">Gwern</a> phrases this idea similarly: <i>Rather, I am attempting to explain things to my future self, who is intelligent and interested, but has forgotten. What I am doing is explaining why I decided what I did to myself and noting down everything I found interesting about it for future reference. I hope my other readers, whomever they may be, might find the topic as interesting as I found it, and the essay useful or at least entertaining–but the intended audience is my future self.<i></p></li>
  </ol>
</details>
