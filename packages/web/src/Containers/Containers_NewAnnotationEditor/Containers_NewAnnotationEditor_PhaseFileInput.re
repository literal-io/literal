open Containers_NewAnnotationEditor_GraphQL;

type phase =
  | PhaseLoading
  | PhaseData(CreateAnnotationFromExternalTargetMutation.t);

[@react.component]
let make = (~currentUser, ~file) => {
  let (createAnnotationFromExternalTargetMutation, _s, _f) =
    ApolloHooks.useMutation(
      CreateAnnotationFromExternalTargetMutation.definition,
    );

  let (phase, setPhase) = React.useState(() => PhaseLoading);

  let _ =
    React.useEffect0(() => {
      let fileId = Uuid.makeV4();
      let _ =
        AwsAmplify.Storage.(
          inst->put(
            "screenshots/" ++ fileId,
            file,
            Some({contentType: file->Webapi.File.type_, level: "private"}),
          )
        )
        |> Js.Promise.then_(result => {
             let creatorUsername =
               currentUser->AwsAmplify.Auth.CurrentUserInfo.username;
             let variables =
               CreateAnnotationFromExternalTargetMutation.makeVariables(
                 ~input={
                   "creatorUsername": creatorUsername,
                   "annotationId":
                     Some(
                       Lib_GraphQL.Annotation.makeIdFromComponent(
                         ~creatorUsername,
                         ~annotationIdComponent=fileId,
                       ),
                     ),
                   "externalTarget": {
                     "format": Some(`TEXT_PLAIN),
                     "language": Some(`EN_US),
                     "processingLanguage": Some(`EN_US),
                     "type": Some(`IMAGE),
                     "rights": None,
                     "accessibility": None,
                     "textDirection": Some(`LTR),
                     "id": result.AwsAmplify.Storage.key,
                   },
                 },
                 (),
               );
             createAnnotationFromExternalTargetMutation(~variables, ());
           });
      None;
    });
  ();
};
