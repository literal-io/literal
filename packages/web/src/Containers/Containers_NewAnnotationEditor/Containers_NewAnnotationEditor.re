[@bs.deriving jsConverter]
type phase = [ | `PhasePrompt | `PhaseTextInput | `PhaseFileInput];
type phaseData = option(Webapi.File.t);
type phaseState = {
  phase,
  data: phaseData,
};

let phase_encode = p => p->phaseToJs->Js.Json.string;
let phase_decode = json =>
  switch (json->Js.Json.decodeString->Belt.Option.flatMap(phaseFromJs)) {
  | Some(p) => Ok(p)
  | None =>
    Error(Decco.{path: "", message: "Not a phase value.", value: json})
  };

[@bs.deriving accessors]
type action =
  | SetPhase(phase)
  | SetPhaseAndData(phase, phaseData);

[@react.component]
let make =
    (~currentUser, ~initialPhaseState=`PhasePrompt, ~initialPhaseData=None) => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) => {
        switch (action) {
        | SetPhase(nextPhase) => {phase: nextPhase, data: None}
        | SetPhaseAndData(nextPhase, data) => {phase: nextPhase, data}
        }
      },
      {phase: initialPhaseState, data: initialPhaseData},
    );

  let handleCreateFromText = () => {
    let _ = `PhaseTextInput->setPhase->dispatchPhaseAction;
    ();
  };
  let handleCreateFromFile = file => {
    let _ =
      dispatchPhaseAction(setPhaseAndData(`PhaseFileInput, Some(file)));
    ();
  };

  switch (phaseState) {
  | {phase: `PhaseTextInput} =>
    <Containers_NewAnnotationEditor_PhaseTextInput currentUser />
  | {phase: `PhaseFileInput, data: Some(file)} =>
    <Containers_NewAnnotationEditor_PhaseFileInput currentUser file />
  | {phase: `PhasePrompt} | _ =>
    <Containers_NewAnnotationEditor_PhasePrompt
      onCreateFromFile=handleCreateFromFile
      onCreateFromText=handleCreateFromText
    />
  };
};
