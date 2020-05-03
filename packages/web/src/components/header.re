open Styles;

[@react.component]
let make = (~children) =>
  <div className={cn(["flex", "flex-row", "h-12", "py-2", "px-6"])}>
    children
  </div>;
