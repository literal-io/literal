module Skeleton = {
  [@bs.module "@material-ui/lab/Skeleton/index.js"] [@react.component]
  external make:
    (
      ~variant: [@bs.string] [ | `text | `circle | `rect],
      ~classes: {. "root": string}=?
    ) =>
    React.element =
    "default";
};
