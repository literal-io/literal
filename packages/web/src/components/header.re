open Styles;

[@react.component]
let make = (~children=React.null, ~className=?, ~style=?) =>
  <div
    ?style
    className={cn([
      "flex",
      "flex-row",
      "h-12",
      Cn.take(className),
    ])}>
    children
  </div>;
