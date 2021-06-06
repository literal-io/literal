module Content = {
  let header = "Literal augments your online reading experience;\ncapture annotations, sources, and knowledge.";

  let howItWorks = [|
    (
      "Share web content to Literal to add it to your library of sources and start annotating it.",
      "/how-it-works-1.png",
    ),
    (
      "As you read, highlight text to create annotations.",
      "/how-it-works-2.png",
    ),
    (
      "Build a knowledge base from your annotations, a library of idea bookmarks that link back to their source context.",
      "/how-it-works-3.png",
    ),
    (
      "Read annotations in their source context, and never lose track or access to sources within your library.",
      "/how-it-works-5.png",
    ),
    (
      "Organize annotations with tags, or view them grouped by their source.",
      "/how-it-works-4.png",
    ),
  |];

  let philosophy = [|
    (
      "A Tool for Digital Reading",
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightSecondary",
          "text-base",
        ])}>
        {React.string(
           "The way that we read has remained relatively unchanged even as the medium that carries text has evolved. E-reader applications are traditionally skeumorphic and seek to make reading digital text more like reading a physical book. Literal is instead designed to support interaction with digital-text in a native way.",
         )}
      </p>,
    ),
    (
      "A Commonplace Book",
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightSecondary",
          "text-base",
        ])}>
        {React.string(
           "The web has led to the proliferation of information at a scale never before seen. Literal enables the practice of ",
         )}
        <UnderlineLink
          href="https://en.wikipedia.org/wiki/Commonplace_book"
          text="commonplacing"
        />
        {React.string(
           " through annotation and source capture, and allows you to build a personal knowledgebase of curated information.",
         )}
      </p>,
    ),
    (
      "A Star in a Constellation",
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightSecondary",
          "text-base",
        ])}>
        {React.string(
           "Literal is a component of a much larger system though which your personal data flows. Literal is ",
         )}
        <UnderlineLink
          text="open source"
          href="https://github.com/literal-io/literal"
        />
        {React.string(
           " to increase trust and prevent proprietary lock-in, and implements the ",
         )}
        <UnderlineLink
          text="W3C Web Annotation Data Model"
          href="https://www.w3.org/TR/annotation-model/"
        />
        {React.string(" to ensure that your data is portable.")}
      </p>,
    ),
  |];

  type feature = {
    enabled: bool,
    title: string,
    description: string,
  };
  let features = [|
    {
      title: "Web Annotation",
      description: "Annotate text while reading to capture ideas, phrases, and knowledge.",
      enabled: true,
    },
    {
      title: "Source Capture & Management",
      description: "Build a digital library. Sources that you read and annotate are automatically archived and indexed.",
      enabled: true,
    },
    {
      title: "Tag Collections",
      description: "Organize annotations and sources with tags to build collections of related ideas.",
      enabled: true,
    },
    {
      title: "Android Application",
      description: "A fully-featured native application that allows for quick capture of sources via the share sheet, annotation, library management, and more.",
      enabled: true,
    },
    {
      title: "Additional Platforms",
      description: "Read anywhere. iOS application, browser extensions, and a web application.",
      enabled: false,
    },
    {
      title: "Augmented Reader",
      description: "An integrated development environment for understanding text.",
      enabled: false,
    },
    {
      title: "PDF Support",
      description: "Vast amounts of digital text is contained within PDF documents.",
      enabled: false,
    },
    {
      title: "Integrations and APIs",
      description: "Rich annotation embeds, Hypothes.is integration, data export, and a full API.",
      enabled: false,
    },
  |];

  type footerLink = {
    title: string,
    description: string,
    href: string,
  };
  let footerLinks = [|
    {
      title: "Install",
      description: "Get the mobile application.",
      href: "/start",
    },
    {title: "Writing", description: "View the blog.", href: "/writing"},
    {
      title: "Contact",
      description: "Reach out to tell us what you think, ask a question, or just say hello.",
      href: "mailto:daniel@literal.io",
    },
    {
      title: "Open Source",
      description: "View the source code.",
      href: "https://github.com/javamonn/literal",
    },
    {
      title: "Privacy Policy",
      description: "What data we collect and why.",
      href: "/policies/privacy",
    },
  |];
};

[@react.component]
let default = () => {
  let (howItWorksVisibleIdx, setHowItWorksVisibleIdx) =
    React.useState(_ => 0);
  let howItWorksContainerRef = React.useRef(Js.Nullable.null);

  let handleHowItWorksScroll = newIdx => {
    let _ =
      howItWorksContainerRef.current
      ->Js.Nullable.toOption
      ->Belt.Option.forEach(scrollContainer => {
          setHowItWorksVisibleIdx(_ => newIdx);
          let _ = scrollContainer##scrollToIdx(~behavior="smooth", newIdx);
          ();
        });
    ();
  };
  let handleHowItWorksVisibleIdxChange = newIdx => {
    setHowItWorksVisibleIdx(_ => newIdx);
  };

  <>
    <header className={Cn.fromList(["pt-12", "px-6", "sm:px-12"])}>
      <Svg icon=Svg.logo className={Cn.fromList(["w-12", "h-12", "block"])} />
    </header>
    <main
      style={ReactDOM.Style.make(~maxWidth="750px", ())}
      className={Cn.fromList(["bg-black", "px-6", "sm:px-12"])}>
      <section className={Cn.fromList(["py-40", "sm:py-56"])}>
        <h1
          className={Cn.fromList([
            "text-2xl",
            "font-serif",
            "text-lightPrimary",
          ])}>
          {React.string(Content.header)}
        </h1>
        <ArrowTextButton
          variant={ArrowTextButton.Link({href: "/start"})}
          action="start cta"
          label="Start Now"
          className={Cn.fromList(["mt-12"])}
          style={ReactDOMRe.Style.make(~opacity="0.72", ())}
        />
      </section>
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
        ])}
      />
      <section className={Cn.fromList(["py-16"])}>
        <h2
          className={Cn.fromList([
            "font-serif",
            "text-lightPrimary",
            "text-xl",
          ])}>
          {React.string("How It Works")}
        </h2>
        <div
          className={Cn.fromList([
            "flex",
            "flex-row",
            "justify-between",
            "mb-8",
            "mt-12",
          ])}>
          <ArrowTextButton
            label="Prev"
            action="how it works"
            className=Cn.(
              on(fromList(["invisible"]), howItWorksVisibleIdx == 0)
            )
            style={ReactDOM.Style.make(~opacity="0.72", ())}
            iconStyle={ReactDOM.Style.make(~transform="rotate(180deg)", ())}
            variant={
              ArrowTextButton.Button({
                onClick: () =>
                  handleHowItWorksScroll(howItWorksVisibleIdx - 1),
              })
            }
          />
          <ArrowTextButton
            label="Next"
            action="how it works"
            style={ReactDOM.Style.make(~opacity="0.72", ())}
            className=Cn.(
              fromList([
                "mb-2",
                on(
                  fromList(["invisible"]),
                  howItWorksVisibleIdx == Js.Array.length(Content.howItWorks)
                  - 1,
                ),
              ])
            )
            variant={
              ArrowTextButton.Button({
                onClick: () =>
                  handleHowItWorksScroll(howItWorksVisibleIdx + 1),
              })
            }
          />
        </div>
        <ScrollSnapList.Container
          ref=howItWorksContainerRef
          onIdxChange=handleHowItWorksVisibleIdxChange
          className={Cn.fromList(["flex", "flex-row", "overflow-x-auto"])}
          direction=ScrollSnapList.Horizontal>
          {Content.howItWorks->Belt.Array.map(((copy, src)) =>
             <ScrollSnapList.Item
               key={src}
               className={Cn.fromList([
                 "flex-shrink-0",
                 "w-full",
                 "sm:mb-12",
               ])}
               direction=ScrollSnapList.Horizontal>
               <p
                 className={Cn.fromList([
                   "font-sans",
                   "text-lightPrimary",
                   "mb-8",
                   "mr-4",
                   "leading-7",
                 ])}
                 style={ReactDOM.Style.make(~minHeight="3.5rem", ())}>
                 {React.string(copy)}
               </p>
               <Next.Image src width=350 height=724 />
             </ScrollSnapList.Item>
           )}
        </ScrollSnapList.Container>
      </section>
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
        ])}
      />
      <section className={Cn.fromList(["py-16"])}>
        <h2
          className={Cn.fromList([
            "font-serif",
            "text-lightPrimary",
            "text-xl",
            "mb-12",
          ])}>
          {React.string("Philosophy")}
        </h2>
        <ul>
          {Content.philosophy
           ->Belt.Array.mapWithIndex((idx, (title, text)) =>
               <li
                 key=title
                 className=Cn.(
                   fromList([
                     on(
                       fromList(["mb-10"]),
                       idx < Js.Array2.length(Content.philosophy) - 1,
                     ),
                   ])
                 )>
                 <h3
                   className={Cn.fromList([
                     "font-serif",
                     "text-lg",
                     "text-lightPrimary",
                     "mb-2",
                   ])}>
                   {React.string(title)}
                 </h3>
                 text
               </li>
             )
           ->React.array}
        </ul>
      </section>
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
        ])}
      />
      <section className={Cn.fromList(["py-16"])}>
        <div
          className={Cn.fromList([
            "flex",
            "flex-row",
            "justify-between",
            "items-center",
            "mb-12",
          ])}>
          <h2
            className={Cn.fromList([
              "font-serif",
              "text-lightPrimary",
              "text-xl",
            ])}>
            {React.string("Features")}
          </h2>
          <div className={Cn.fromList(["flex", "flex-col", "justify-end"])}>
            <div
              className={Cn.fromList([
                "flex",
                "flex-row",
                "items-center",
                "mb-2",
              ])}>
              <FeatureIndicator enabled=true />
              <h3 className={Cn.fromList(["font-sans", "text-lightPrimary"])}>
                {React.string("Available Now")}
              </h3>
            </div>
            <div
              className={Cn.fromList(["flex", "flex-row", "items-center"])}>
              <FeatureIndicator enabled=false />
              <h3
                className={Cn.fromList(["font-sans", "text-lightDisabled"])}>
                {React.string("Coming Soon")}
              </h3>
            </div>
          </div>
        </div>
        <ul>
          {Content.features
           ->Belt.Array.mapWithIndex(
               (idx, {enabled, title, description}: Content.feature) =>
               <FeatureListItem
                 key=title
                 enabled
                 title
                 description
                 className=Cn.(
                   fromList([
                     on(
                       fromList(["mb-8"]),
                       idx < Js.Array2.length(Content.features) - 1,
                     ),
                   ])
                 )
               />
             )
           ->React.array}
        </ul>
      </section>
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
        ])}
      />
      <section className={Cn.fromList(["py-16"])}>
        <ul>
          {Content.footerLinks
           ->Belt.Array.mapWithIndex(
               (idx, {title, description, href}: Content.footerLink) =>
               <li
                 key=title
                 className=Cn.(
                   fromList([
                     on(
                       fromList(["mb-8"]),
                       idx < Js.Array2.length(Content.footerLinks),
                     ),
                   ])
                 )>
                 <FooterLink title description href />
               </li>
             )
           ->React.array}
        </ul>
      </section>
    </main>
  </>;
};

let page = "index.js";
