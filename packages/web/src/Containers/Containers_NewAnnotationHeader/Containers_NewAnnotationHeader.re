open Styles;

[@react.component]
let make = () => {
  let handleClose = () => {
    let _ = Next.Router.back();
    ();
  };

  <Header className={cn(["py-2", "mx-6"])}>
    <MaterialUi.IconButton
      size=`Small
      edge=MaterialUi.IconButton.Edge.start
      onClick={_ => handleClose()}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(~root=cn(["p-0"]), ())}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["w-8", "h-8", "pointer-events-none", "opacity-75"])}
        icon=Svg.close
      />
    </MaterialUi.IconButton>
  </Header>;
};
