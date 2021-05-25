open Styles;

[@react.component]
let make = (~identityId, ~initialPhaseState=?) =>
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
    <Containers_NewAnnotationEditor identityId ?initialPhaseState />
  </div>;
