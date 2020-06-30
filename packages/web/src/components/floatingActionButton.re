open Styles;

[@react.component]
let make = (~onClick=?, ~className=?, ~disabled=?, ~children) =>
  <div ?className>
    <MaterialUi.Fab ?onClick classes=[Root(cn(["bg-white"]))] ?disabled>
      children
      <div
        className={cn([
          "h-14",
          "w-14",
          "absolute",
          "top-0",
          "left-0",
          "mt-1/2",
          "ml-1/2",
          "border",
          "border-white",
          "rounded-full",
        ])}
      />
    </MaterialUi.Fab>
  </div>;
