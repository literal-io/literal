open Containers_NewAnnotationEditor_Types;

let noDataAlert = "Unable to parse image. Make sure the text is clearly highlighted and try again, or enter the text manually.";

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

  let handleError = _ => {
    let _ = `PhasePrompt->setPhase->dispatchPhaseAction;
    let search =
      Redirect.encodeSearch(Alert.(query_encode({alert: noDataAlert})));

    let _ =
      Next.Router.replaceWithAs(
        Routes.CreatorsIdAnnotationsNew.staticPath,
        Routes.CreatorsIdAnnotationsNew.path(
          ~creatorUsername=
            currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        )
        ++ search,
      );
    ();
  };

  switch (phaseState) {
  | {phase: `PhaseTextInput} =>
    <Containers_NewAnnotationEditor_PhaseTextInput currentUser />
  | {phase: `PhaseFileInput, data: Some(file)} =>
    <Containers_NewAnnotationEditor_PhaseFileInput
      currentUser
      file
      onError=handleError
    />
  | {phase: `PhasePrompt}
  | _ =>
    <Containers_NewAnnotationEditor_PhasePrompt
      onCreateFromFile=handleCreateFromFile
      onCreateFromText=handleCreateFromText
    />
  };
};
