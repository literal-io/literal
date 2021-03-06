open Styles;

[@react.component]
let make = () => {
  let handleClose = () => {
    let _ = Next.Router.back();
    ();
  };

  <Header
    className={Cn.fromList([
      "h-14",
      "py-2",
      "px-4",
      "bg-darkAccent",
      "border-b",
      "border-dotted",
      "border-lightDisabled",
      "flex",
      "flex-row",
      "items-center"
    ])}>
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge.start
      onClick={_ => {
        let _ =
          Service_Analytics.(track(Click({action: "close", label: None})));
        handleClose();
      }}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(~root=cn(["p-0"]), ())}>
      <Svg
        className={cn(["w-8", "h-8", "pointer-events-none", "opacity-75"])}
        icon=Svg.close
      />
    </MaterialUi.IconButton>
    <h1
      className={Cn.fromList([
        "font-sans",
        "font-bold",
        "text-lg",
        "text-lightPrimary",
        "ml-4",
      ])}>
      {React.string("Create Annotation")}
    </h1>
  </Header>;
};
