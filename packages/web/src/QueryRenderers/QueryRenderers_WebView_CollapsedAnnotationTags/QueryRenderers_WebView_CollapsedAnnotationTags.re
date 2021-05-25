let tagsValueSelector = (~annotation: LiteralModel.Annotation.t, ~identityId) =>
  annotation.body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body =>
        switch (body) {
        | LiteralModel.Body.TextualBody(body) =>
          let href =
            Lib_GraphQL.AnnotationCollection.(
              makeIdFromComponent(
                ~annotationCollectionIdComponent=idComponent(body.id),
                ~identityId,
                ~origin=Webapi.Dom.(window->Window.location->Location.origin),
                (),
              )
            );
          Some(
            Containers_AnnotationEditor_Tag.{
              text: body.value,
              id: Some(body.id),
              href: Some(href),
            },
          );
        | _ => None
        }
      )
    )
  ->Belt.Option.getWithDefault([||])
  ->Belt.Array.keep(({text}) =>
      text != Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel
    );

module Loading = {
  [@react.component]
  let make = () => {
    <div />;
  };
};

[@react.component]
let make = (~identityId, ~annotation, ~onExpand) => {
  let tags = tagsValueSelector(~annotation, ~identityId);

  let tagsList =
    Js.Array2.length(tags) > 0
      ? <ul className={Cn.fromList(["flex-1", "overflow-x-auto", "py-2"])}>
          {tags
           ->Belt.Array.mapWithIndex(
               (idx, tag: Containers_AnnotationEditor_Tag.t) =>
               <div
                 key={tag.text}
                 className={Cn.fromList([
                   Cn.on("ml-6", idx == 0),
                   "mr-6",
                   "border-b",
                   "border-dotted",
                   "border-lightPrimary",
                   "inline",
                 ])}>
                 <span
                   className={Cn.fromList([
                     "font-sans",
                     "text-lightSecondary",
                     "text-base",
                     "normal-case",
                   ])}>
                   {React.string(tag.text)}
                 </span>
               </div>
             )
           ->React.array}
        </ul>
      : <div
          className={Cn.fromList(["flex-1", "items-center", "flex"])}
          onClick={_ => onExpand()}>
          <span
            className={Cn.fromList([
              "font-sans",
              "font-medium",
              "text-lightDisabled",
              "text-sm",
              "italic",
              "ml-6",
            ])}>
            {React.string("Add Tag...")}
          </span>
        </div>;

  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-row",
    ])}>
    tagsList
    <div
      onClick={_ => onExpand()}
      className={Cn.fromList([
        "flex",
        "justify-center",
        "items-center",
        "h-12",
        "w-12",
        "border-l",
        "border-lightPrimary",
        "border-dotted",
      ])}>
      <Svg
        className={Cn.fromList(["w-8", "h-8", "pointer-events-none"])}
        icon=Svg.arrowUp
      />
    </div>
  </div>;
};
