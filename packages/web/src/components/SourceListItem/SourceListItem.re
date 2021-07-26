let styles = [%raw "require('./SourceListItem.module.css')"];
open SourceListItem_GraphQL;

module Loading = {
  [@react.component]
  let make = () => {
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "p-4",
      ])}>
      <div className={Cn.fromList(["flex", "flex-col", "flex-1", "mr-4"])}>
        <Skeleton
          variant=`text
          className={Cn.fromList(["h-4", "w-40", "mb-3", "transform-none"])}
        />
        <Skeleton
          variant=`text
          className={Cn.fromList(["h-4", "mr-12", "transform-none"])}
        />
      </div>
      <div className={Cn.fromList(["flex", "flex-col"])}>
        <Skeleton
          variant=`text
          className={Cn.fromList(["h-4", "w-4", "transform-none"])}
        />
      </div>
    </div>;
  };
};

[@react.component]
let make = (~annotationCollectionFragment as annotationCollection, ~onClick=?) => {
  let url = annotationCollection##label->Webapi.Url.make;

  <Next.Link
    passHref=true
    _as={annotationCollection##id->Webapi.Url.make->Webapi.Url.pathname}
    href=Routes.CreatorsIdAnnotationCollectionsId.staticPath>
    <a
      ?onClick
      className={Cn.fromList([
        "flex",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
      ])}>
      <MaterialUi.ButtonBase
        classes={MaterialUi.ButtonBase.Classes.make(
          ~root=
            Cn.fromList([
              "p-4",
              "flex",
              "flex-row",
              "justify-start",
              "items-start",
              "bg-darkAccent",
              "flex-1",
            ]),
          (),
        )}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-50"]),
          },
        }>
        <div
          className={Cn.fromList([
            "flex",
            "flex-col",
            "flex-1",
            "mr-4",
            "items-start",
          ])}>
          <h3
            className={Cn.fromList([
              "text-lightPrimary",
              "font-sans",
              "text-left",
              "leading-none",
              "mb-3",
            ])}>
            {url->Webapi.Url.host->React.string}
          </h3>
          <h4
            className={Cn.fromList([
              styles##pathname,
              "text-lightSecondary",
              "font-sans",
              "leading-none",
              "text-left",
              "text-sm",
            ])}>
            {url->Webapi.Url.pathname->React.string}
          </h4>
        </div>
        <div className={Cn.fromList(["flex", "flex-col"])}>
          <span
            className={Cn.fromList([
              "text-lightDisabled",
              "font-sans",
              "leading-none",
            ])}>
            {annotationCollection##total
             ->Belt.Option.getWithDefault(1)
             ->React.int}
          </span>
        </div>
      </MaterialUi.ButtonBase>
    </a>
  </Next.Link>;
};
