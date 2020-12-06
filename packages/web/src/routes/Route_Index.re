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
        className={Cn.fromList(["font-sans", "text-lightSecondary", "mb-8"])}>
        {React.string(
           "The act of annotation is only the beginning. With Literal, your reading highlights can be revisted and reexaminded, tended as an epistemological garden.",
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
      "Contextualize and associate.",
      "Ideas do not exist in isolation. Ideas are threads that span otherwise disparate contexts. Annotations organized strictly by their original source is an artificial limitation. Create bridges, not silos.",
    ),
    (
      "Galleries for thought.",
      "The act of annotation is only the beginning. Ideas should be revisited and reexamined, tended as epistemological gardens.",
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
        {React.string("Why Literal")}
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
      "Highlight Text",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
    ),
    (
      "Highlight Text",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
    ),
    (
      "Highlight Text",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
    ),
    (
      "Highlight Text",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
    ),
    (
      "Highlight Text",
      "Literal supports annotations made within web browsers, PDF readers, and more.",
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
                 className={Cn.fromList(["font-serif", "text-lightPrimary"])}>
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
      description: "Reach out to tell us what you think, ask a question, or just say hi.",
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
    ->Belt.Array.map(({title, description, href}) =>
        <a
          className={Cn.fromList([
            "block",
            "border",
            "border-dotted",
            "border-lightDisabled",
            "p-4",
            "mb-6",
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
    className={cn([
      "w-full",
      "h-full",
      "overflow-y-scroll",
      "flex",
      "flex-col",
      "bg-white",
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
          "mb-6",
        ])}
      />
      <Footer />
    </div>
  </div>;
};

let page = "index.js";
