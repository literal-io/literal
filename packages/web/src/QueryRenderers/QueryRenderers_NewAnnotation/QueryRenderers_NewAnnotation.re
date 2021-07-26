open Styles;

[@react.component]
let make = (~identityId, ~initialPhaseState=?) =>
  <>
    <Containers_NewAnnotationHeader />
    <Containers_NewAnnotationEditor identityId ?initialPhaseState />
  </>
