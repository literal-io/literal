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
    <Containers_NewNoteHeader />
    <Containers_NoteEditor_New currentUser ?initialPhaseState />
  </div>;
