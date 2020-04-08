open Styles;

[@react.component]
let make = (~children) =>
  <div className={cn(["flex", "flex-row", "py-2", "px-6"])}> children </div>;
