let styles = [%raw "require('./Skeleton.module.css')"];

[@react.component]
let make = (~variant, ~className=?) =>
  <MaterialUiLab.Skeleton
    variant
    classes={"root": Cn.fromList([styles##skeleton, Cn.take(className)])}
  />;
