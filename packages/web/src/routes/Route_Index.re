open Styles;

let styles = [%raw "require('./Route_Index.module.css')"];

let waitlistFormUrl = "https://docs.google.com/forms/d/1S6xFRp80nYYvPcz9oQYmiyOqLyXKHPZ0D2LofOxgWeo/edit";

module Header = {
  [@react.component]
  let make = () =>
    <>
      <Svg
        icon=Svg.logo
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "w-12", "h-12", "block"])}
      />
      <h1
        className={cn([
          "text-lightPrimary",
          "font-serif",
          "text-2xl",
          "leading-none",
          "mb-8",
          "mt-20",
        ])}>
        {React.string("Literal is a textual annotation management system.")}
      </h1>
      <p
        className={Cn.fromList(["font-sans", "text-lg", "text-lightSecondary", "mb-8"])}>
        {React.string(
           "The act of annotation is only the beginning. Leverage your highlights to enhance your reading experience.",
         )}
      </p>
      <a
        onClick={_ => {
          let _ =
            Service_Analytics.(
              track(Click({action: "start cta", label: Some("Start now")}))
            );
          ();
        }}
        href=waitlistFormUrl
        className={cn([
          "text-lightPrimary",
          "text-lg",
          "font-sans",
          "flex",
          "flex-row",
          "mb-40",
          "items-center",
        ])}>
        <Svg
          icon=Svg.arrowRight
          placeholderViewBox="0 0 32 32"
          className={cn(["pointer-events-none", "w-8", "h-8"])}
        />
        <div>
          <span
            className={Cn.fromList([
              "italic",
              "border-b",
              "border-dotted",
              "border-lightPrimary",
              "block",
              "leading-none",
            ])}>
            {React.string("Start Now")}
          </span>
        </div>
      </a>
    </>;
};

module WhyLiteral = {
  let items = [|
    (
      "Separate signal from noise.",
      "Interacting with text elevates the reading experience beyond one of passive consumption and into one that requires analysis and critical thought. Annotations are of equal value to the text itself.",
    ),
    (
      "Create bridges, not silos.",
      "Ideas do not exist in isolation. Ideas are threads that span otherwise disparate contexts. Annotations organized strictly by their original source is an artificial limitation.",
    ),
    (
      "Understand the source.",
      "Annotations removed from their original context are like a verse without a song, or a headline without an article. Given a highlight, you should frequently reexamine it within its original context.",
    ),
    (
      "Open by default.",
      "Information flows like ripples in a pond, and cascades to create and amplify network effects. Data - annotations and sources, creators and consumers - should be free to move across the boundaries of systems.",
    ),
  |];
  [@react.component]
  let make = () =>
    <>
      <h2
        className={Cn.fromList([
          "text-lightPrimary",
          "font-serif",
          "text-2xl",
          "leading-none",
          "mb-16",
        ])}>
        {React.string("Precepts")}
      </h2>
      {items
       ->Belt.Array.map(((header, body)) =>
           <React.Fragment key=header>
             <h2
               className={cn([
                 "text-lightPrimary",
                 "font-serif",
                 "text-xl",
                 "mb-2",
               ])}>
               {React.string(header)}
             </h2>
             <p
               className={cn([
                 "text-lightSecondary",
                 "font-sans",
                 "mb-14",
                 "block",
               ])}>
               {React.string(body)}
             </p>
           </React.Fragment>
         )
       ->React.array}
    </>;
};

module HowItWorks = {
  let items = [|
    (
      "Highlight Text.",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
    ),
    (
      "Import the Highlight.",
      "Share the highlight with Literal to import it. Use the selection toolbar to share the highlighted text directly, or share a screenshot of the highlight.",
    ),
    (
      "Annotate the Highlight.",
      "Add context and thoughts by adding tags and notes.",
    ),
    (
      "Explore.",
      "Browse annotations grouped by tags and reflect as ideas evolve in response to new annotations and new relationships between them.",
    ),
  |];

  [@react.component]
  let make = () =>
    <>
      <h2
        className={Cn.fromList([
          "text-lightPrimary",
          "font-serif",
          "text-2xl",
          "leading-none",
        ])}>
        {React.string("How It Works")}
      </h2>
      <ul
        className={Cn.fromList([
          "overflow-x-scroll",
          "flex",
          "py-16",
          styles##scrollContainer,
        ])}
        style={
          style(~width="calc(100% + 2rem)", ())
          ->unsafeStyle([
              ("scrollSnapType", "x mandatory"),
              ("scrollSnapStop", "always"),
            ])
        }>
        {items
         ->Belt.Array.mapWithIndex((idx, (title, body)) =>
             <div
               style={
                 style(
                   ~width=
                     idx === Belt.Array.length(items) - 1
                       ? "calc(100% - 2rem)" : "calc(85%)",
                   (),
                 )
                 ->unsafeStyle([
                     ("scrollSnapStop", "always"),
                     ("scrollSnapAlign", "start"),
                   ])
               }
               className={Cn.fromList([
                 "border",
                 "border-lightDisabled",
                 "border-dotted",
                 "p-6",
                 "flex-shrink-0",
                 "mr-4",
               ])}>
               <h3
                 className={Cn.fromList([
                   "font-serif",
                   "text-lightPrimary",
                   "mb-4",
                 ])}>
                 {React.string(string_of_int(idx + 1) ++ ". " ++ title)}
               </h3>
               <p
                 className={Cn.fromList(["font-sans", "text-lightSecondary"])}>
                 {React.string(body)}
               </p>
             </div>
           )
         ->React.array}
      </ul>
    </>;
};

module Features = {
  type item = {
    title: string,
    available: bool,
  };
  let items = [|
    {title: "Android mobile app", available: true},
    {title: "Create highlight from screenshot", available: true},
    {title: "Create highlight from text", available: true},
    {title: "Annotation tagging", available: true},
    {title: "First-class W3C Web Annotation support", available: true},
    {title: "iOS mobile app", available: false},
    {title: "Annotation source viewer", available: false},
    {title: "Graph viewer", available: false},
    {title: "Browser extension", available: false},
    {title: "Annotation sharing and discovery", available: false},
  |];

  let renderFeature = (~text, ~available, ~className=?, ()) =>
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "items-center",
        Cn.take(className),
      ])}>
      <div
        className={Cn.fromList([
          "mr-4",
          "w-4",
          "h-4",
          available
            ? Cn.fromList(["bg-lightPrimary"])
            : Cn.fromList(["border-lightDisabled", "border", "border-dotted"]),
        ])}
      />
      <p
        className={Cn.fromList([
          available
            ? Cn.fromList(["text-lightPrimary"])
            : Cn.fromList(["text-lightDisabled"]),
        ])}>
        {React.string(text)}
      </p>
    </div>;

  [@react.component]
  let make = () =>
    <>
      <div
        className={Cn.fromList([
          "flex",
          "flex-row",
          "justify-between",
          "items-center",
          "mb-16",
        ])}>
        <h2
          className={Cn.fromList([
            "text-lightPrimary",
            "font-serif",
            "text-2xl",
            "leading-none",
          ])}>
          {React.string("Features")}
        </h2>
        <div className={Cn.fromList(["flex", "flex-col"])}>
          {renderFeature(
             ~text="Available Now",
             ~available=true,
             ~className="mb-2",
             (),
           )}
          {renderFeature(~text="Coming Soon", ~available=false, ())}
        </div>
      </div>
      <p className={Cn.fromList(["text-lightSecondary", "mb-16"])}>
        {React.string(
           "We're just getting started, and there is so much work to do. Get started now, and keep in mind that we're always improving.",
         )}
      </p>
      <ul className={Cn.fromList(["mb-16"])}>
        {items
         ->Belt.Array.map(({title, available}) =>
             <li>
               {renderFeature(
                  ~text=title,
                  ~available,
                  ~className=Cn.fromList(["mb-4"]),
                  (),
                )}
             </li>
           )
         ->React.array}
      </ul>
    </>;
};

module Footer = {
  type item = {
    title: string,
    description: string,
    href: string,
  };

  let items = [|
    {
      title: "Install",
      description: "Get the mobile application.",
      href: waitlistFormUrl,
    },
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
  |];

  [@react.component]
  let make = () =>
    items
    ->Belt.Array.mapWithIndex((idx, {title, description, href}) =>
        <a
          className={Cn.fromList([
            "block",
            "border",
            "border-dotted",
            "border-lightDisabled",
            "p-4",
            Cn.on("mt-16", idx === 0),
            Cn.on("mb-6", idx !== Belt.Array.length(items) - 1),
          ])}
          href>
          <div
            className={Cn.fromList([
              "flex",
              "flex-row",
              "items-center",
              "justify-between",
            ])}>
            <div className={Cn.fromList(["flex", "flex-col"])}>
              <h3
                className={Cn.fromList([
                  "font-serif",
                  "text-lightPrimary",
                  "mb-2",
                ])}>
                {React.string(title)}
              </h3>
              <p
                className={Cn.fromList([
                  "font-sans",
                  "text-lightSecondary",
                  "leading-tight",
                ])}>
                {React.string(description)}
              </p>
            </div>
            <Svg
              icon=Svg.arrowRight
              placeholderViewBox="0 0 48 48"
              className={cn([
                "pointer-events-none",
                "w-12",
                "h-12",
                "opacity-75",
              ])}
            />
          </div>
        </a>
      )
    ->React.array;
};

[@react.component]
let default = () => {
  <div
    style={style(~backgroundColor="rgb(229, 229, 229)", ())}
    className={cn([
      "w-full",
      "h-full",
      "overflow-y-scroll",
      "flex",
      "flex-col",
    ])}>
    <div className={cn(["p-8", "bg-black", "max-w-lg", "m-auto", "w-full"])}>
      <Header />
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
          "mb-16",
        ])}
      />
      <HowItWorks />
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
          "mb-16",
        ])}
      />
      <WhyLiteral />
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
          "mb-16",
        ])}
      />
      <Features />
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
          "mb-16",
        ])}
      />
      <Footer />
    </div>
  </div>;
};

let page = "index.js";
