module Loading = {
  [@react.component]
  let make = () => {
    let (width, _setWidth) =
      React.useState(() => {
        let widths = [|"w-20", "w-24", "w-32", "w-40"|];
        Belt.Array.get(
          widths,
          Js.Math.random_int(0, Js.Array2.length(widths)),
        )
        ->Belt.Option.getExn;
      });

    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "p-3",
      ])}>
      <Skeleton
        variant=`text
        className={Cn.fromList(["h-4", width, "transform-none"])}
      />
      <Skeleton
        variant=`text
        className={Cn.fromList(["w-4", "h-4", "ml-3", "transform-none"])}
      />
    </div>;
  };
};

[@react.component]
let make = (~annotationCollectionFragment as annotationCollection, ~onClick) => {
  <Next.Link
    passHref=true
    _as={annotationCollection##id->Webapi.Url.make->Webapi.Url.pathname}
    href=Routes.CreatorsIdAnnotationCollectionsId.staticPath>
    <a
      onClick
      className={Cn.fromList([
        "flex",
        "flex-grow-0",
        "flex-row",
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
      ])}>
      <MaterialUi.ButtonBase
        classes={MaterialUi.ButtonBase.Classes.make(
          ~root=Cn.fromList(["p-3"]),
          (),
        )}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-50"]),
          },
        }>
        <h3
          className={Cn.fromList([
            "text-lightPrimary",
            "font-sans",
            "leading-none",
          ])}>
          {React.string(annotationCollection##label)}
        </h3>
        <span
          className={Cn.fromList([
            "text-lightDisabled",
            "font-sans",
            "leading-none",
            "ml-3",
          ])}>
          {annotationCollection##total
           ->Belt.Option.getWithDefault(1)
           ->React.int}
        </span>
      </MaterialUi.ButtonBase>
    </a>
  </Next.Link>;
};
