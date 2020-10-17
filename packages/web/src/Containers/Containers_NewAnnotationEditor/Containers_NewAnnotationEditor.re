open Containers_NewAnnotationEditor_Types;

let noDataAlert = "Unable to parse image. Make sure the text is clearly highlighted and try again, or enter the text manually.";

[@bs.deriving accessors]
type action =
  | SetPhase(phase);

[@react.component]
let make = (~currentUser, ~initialPhaseState=`PhasePrompt) => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (state, action) => {
        switch (action) {
        | SetPhase(nextPhase) => nextPhase
        }
      },
      initialPhaseState,
    );

  let handleCreateFromText = () => {
    let _ = `PhaseTextInput->setPhase->dispatchPhaseAction;
    ();
  };
  let handleCreateFromFile = file => {
    let fileUrl = Webapi.Url.createObjectURL(file);
    let search =
      "?"
      ++ Routes.CreatorsIdAnnotationsNew.(
           searchParams_encode({id: None, fileUrl: Some(fileUrl)})
         )
         ->Externals_URLSearchParams.makeWithJson
         ->Externals_URLSearchParams.toString;
    let _ =
      Next.Router.pushWithAs(
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
  | `PhaseTextInput =>
    <Containers_NewAnnotationEditor_PhaseTextInput currentUser />
  | `PhasePrompt =>
    <Containers_NewAnnotationEditor_PhasePrompt
      onCreateFromFile=handleCreateFromFile
      onCreateFromText=handleCreateFromText
    />
  };
};
