module TagCollectionTitle = {
  [@react.component]
  let make = (~title) => {
    <span
      className={Cn.fromList([
        "ml-2",
        "block",
        "font-sans",
        "text-lightPrimary",
        "whitespace-no-wrap",
        "overflow-x-hidden",
        "truncate",
        "font-bold",
        "text-lg",
      ])}>
      {React.string(title)}
    </span>;
  };
};

module SourceCollectionTitle = {
  [@react.component]
  let make = (~title, ~subtitle) => {
    <div
      className={Cn.fromList(["ml-3", "flex", "flex-col", "overflow-hidden"])}>
      <span
        className={Cn.fromList([
          "block",
          "font-sans",
          "text-lightPrimary",
          "whitespace-no-wrap",
          "overflow-x-hidden",
          "truncate",
          "font-bold",
          "text-base",
        ])}>
        {React.string(title)}
      </span>
      <span
        className={Cn.fromList([
          "text-lightSecondary",
          "text-xs",
          "overflow-x-hidden",
          "truncate",
        ])}>
        {React.string(subtitle)}
      </span>
    </div>;
  };
};

[@react.component]
let make = (~annotationCollection=?) => {
  annotationCollection
  ->Belt.Option.map(annotationCollection => {
      let type_ =
        annotationCollection##type_
        ->Belt.Array.getBy(t =>
            switch (t) {
            | `TAG_COLLECTION
            | `SOURCE_COLLECTION => true
            | _ => false
            }
          )
        ->Belt.Option.getWithDefault(`TAG_COLLECTION);

      let (title, icon) =
        switch (type_) {
        | `SOURCE_COLLECTION =>
          let url = annotationCollection##label->Webapi.Url.make;
          (
            <SourceCollectionTitle
              title={url->Webapi.Url.host}
              subtitle={url->Webapi.Url.pathname}
            />,
            Svg.article,
          );
        | _ => (
            <TagCollectionTitle title={annotationCollection##label} />,
            Svg.label,
          )
        };

      <div
        className={Cn.fromList([
          "flex",
          "flex-grow-0",
          "overflow-hidden",
          "items-center",
        ])}>
        <Svg
          className={Cn.fromList(["pointer-events-none", "opacity-75"])}
          style={ReactDOMRe.Style.make(~width="1rem", ~height="1rem", ())}
          icon
        />
        title
      </div>;
    })
  ->Belt.Option.getWithDefault(
      <Skeleton
        variant=`text
        className={Cn.fromList(["h-4", "w-32", "ml-6", "transform-none"])}
      />,
    );
};
