open Styles;

[@react.component]
let make = (~currentUser, ~initialPhaseState=?) =>
  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "relative",
      "overflow-y-auto",
    ])}>
    <Containers_NewAnnotationHeader />
    <Containers_AnnotationEditor_New currentUser ?initialPhaseState />
  </div>;
