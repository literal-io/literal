[@react.component]
let make = (~onDismiss) => {
  <Header className={Cn.fromList(["py-2", "mx-6", "flex", "flex-row", "items-center"])}>
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge.start
      onClick={_ => onDismiss()}
      _TouchRippleProps={
        "classes": {
          "child": Cn.fromList(["bg-white"]),
          "rippleVisible": Cn.fromList(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(
        ~root=Cn.fromList(["p-0"]),
        (),
      )}>
      <Svg
        className={Cn.fromList(["w-8", "h-8", "pointer-events-none"])}
        icon=Svg.arrowDown
      />
    </MaterialUi.IconButton>
    <h1
      className={Cn.fromList([
        "text-lightPrimary",
        "font-sans",
        "italic",
        "leading-none",
        "text-base",
        "ml-6",
      ])}>
      {React.string("Edit Annotation")}
    </h1>
  </Header>;
};
