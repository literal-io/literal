open Containers_NewAnnotationEditor_GraphQL;

type phase =
  | PhaseLoading
  | PhaseData(CreateAnnotationFromExternalTargetMutation.t);

module PhaseLoading = {
  [@react.component]
  let make = () =>
    <>
      <TextInput_Loading className={Cn.fromList(["px-6", "pb-4", "pt-16"])} />
      <Containers_NewTagInput />
    </>;
};

module PhaseError = {
  [@react.component]
  let make = (~onError) => {
    let _ =
      React.useEffect0(() => {
        let _ = onError();
        None;
      });
    <PhaseLoading />;
  };
};

[@react.component]
let make = (~currentUser, ~file, ~onError) => {
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
             let s3Url = {
               let bucketName =
                 AwsAmplify.Config.(
                   Constants.awsAmplifyConfig->userFilesS3BucketGet
                 );
               let currentUserId =
                 currentUser
                 ->AwsAmplify.Auth.CurrentUserInfo.id
                 ->Belt.Option.getExn;

               "s3://"
               ++ bucketName
               ++ "/private/"
               ++ currentUserId
               ++ "/"
               ++ result.AwsAmplify.Storage.key;
             };

             Js.log2(s3Url, currentUser);

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
                     "id": s3Url,
                   },
                 },
                 (),
               );
             createAnnotationFromExternalTargetMutation(~variables, ());
           })
        |> Js.Promise.then_(((result, _)) => {
             let _ =
               setPhase(currentPhase =>
                 switch (result) {
                 | ApolloHooks.Mutation.Errors(errors) =>
                   errors->Belt.Array.forEach(error => {
                     Error.(report(GraphQLError(error)))
                   });
                   onError();
                   currentPhase;
                 | NoData =>
                   onError();
                   currentPhase;
                 | Data(result) => PhaseData(result)
                 }
               );
             Js.Promise.resolve();
           })
        |> Js.Promise.catch(_ => {
             let _ = onError();
             Js.Promise.resolve();
           });
      None;
    });

  switch (phase) {
  | PhaseLoading => <PhaseLoading />
  | PhaseData(data) =>
    switch (data##createAnnotationFromExternalTarget) {
    | Some(data) =>
      <Redirect
        path={Routes.CreatorsIdAnnotationsNew.path(
          ~creatorUsername=currentUser.username,
        )}
        staticPath=Routes.CreatorsIdAnnotationsNew.staticPath
        search=Routes.CreatorsIdAnnotationsNew.(
          searchParams_encode({
            id: data##id->Lib_GraphQL.Annotation.idComponent->Js.Option.some,
          })
        )>
        <PhaseLoading />
      </Redirect>

    | None => <PhaseError onError />
    }
  };
};
