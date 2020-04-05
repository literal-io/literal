open Styles;

[@react.component]
let make = (~children=?) => {
  <span
    className={cn([
      "bg-white",
      "text-black",
      "font-serif",
      "text-lg",
      "leading-relaxed",
      "box-decoration-break-clone",
      "px-1",
    ])}
    style={ReactDOMRe.Style.make(
      ~paddingTop="0.125rem",
      ~paddingBottom="0.125rem",
      (),
    )}>
    {switch (children) {
     | Some(children) => children
     | None => React.null
     }}
  </span>;
};
