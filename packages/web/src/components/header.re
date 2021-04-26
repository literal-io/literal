open Styles;

[@react.component]
let make = (~children=React.null, ~className=?, ~style=?) =>
  <nav
    ?style
    className={cn([
      Cn.take(className),
      "flex",
      "flex-row",
      "h-12",
    ])}>
    children
  </nav>;
