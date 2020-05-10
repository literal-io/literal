open Styles;

[@react.component]
let make = (~children=React.null, ~className=?) =>
  <div
    className={cn([
      "flex",
      "flex-row",
      "h-12",
      "py-2",
      "px-6",
      Cn.unpack(className),
    ])}>
    children
  </div>;
