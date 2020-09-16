open Styles;
open QueryRenderers_NewAnnotationFromShare_GraphQL;

let pollInterval = 500;
let pollTimeout = 15 * 1000;
let noDataAlert = "Unable to parse image. Make sure the text is clearly highlighted and try again, or enter the text manually.";

module Data = {
  [@react.component]
  let make = (~annotation, ~currentUser) => {
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
      <Containers_NewAnnotationFromShareHeader
        currentUser
        annotationFragment={annotation##headerAnnotationFragment}
      />
      <Containers_AnnotationEditor_NewFromShare
        currentUser
        annotationFragment={annotation##editorAnnotationFragment}
      />
    </div>;
  };
};

module Loading = {
  [@react.component]
  let make = () => {
    <>
      <Containers_NewAnnotationFromShareHeader />
      <TextInput_Loading className={cn(["px-6", "pb-4", "pt-16"])} />
      <FloatingActionButton
        className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}
        disabled=true>
        <MaterialUi.CircularProgress
          size={MaterialUi.CircularProgress.Size.int(26)}
          classes={MaterialUi.CircularProgress.Classes.make(
            ~colorPrimary=cn(["text-black"]),
            (),
          )}
        />
      </FloatingActionButton>
    </>;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Not Found...");
};

[@react.component]
let make =
    (
      ~annotationId,
      ~authentication: Hooks_CurrentUserInfo_Types.state,
      ~rehydrated,
    ) => {
  let (isLoaded, setIsLoaded) = React.useState(_ => false);
  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=
        GetAnnotationQuery.makeVariables(
          ~id=annotationId,
          ~creatorUsername=
            switch (authentication) {
            | Authenticated(currentUser) =>
              AwsAmplify.Auth.CurrentUserInfo.(currentUser->username)
            | _ => ""
            },
          (),
        ),
      ~pollInterval=isLoaded ? 0 : pollInterval,
      ~skip=
        switch (authentication) {
        | Authenticated(_) when rehydrated => false
        | _ when !rehydrated => true
        | Loading
        | Unauthenticated => true
        },
      GetAnnotationQuery.definition,
    );

  let _ =
    React.useEffect0(() => {
      let timeoutId =
        Js.Global.setTimeout(() => setIsLoaded(_ => true), pollTimeout);
      Some(() => {Js.Global.clearTimeout(timeoutId)});
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (query) {
          | Data(data) =>
            switch (data##getAnnotation) {
            | Some(_) => setIsLoaded(_ => true)
            | _ => ()
            }
          | _ => ()
          };
        None;
      },
      [|query|],
    );

  switch (query, rehydrated, authentication, isLoaded) {
  | (Loading, _, _, _)
  | (_, false, _, _)
  | (_, _, Loading, _)
  | (_, _, _, false) => <Loading />
  | (Data(data), _, Authenticated(currentUser), true) =>
    switch (data##getAnnotation) {
    | Some(annotation) => <Data annotation currentUser />
    | None =>
      <Redirect
        path={Routes.CreatorsIdAnnotationsNew.path(
          ~creatorUsername=currentUser.username,
        )}
        query={Raw.merge(
          Alert.(query_encode({alert: noDataAlert})),
          Routes.CreatorsIdAnnotationsNew.queryParams_encode({
            id: None,
            initialPhaseState: Some(`PhaseTextInput),
            creatorUsername: currentUser.username,
          }),
        )}
      />
    }
  | (NoData, true, Authenticated(currentUser), _)
  | (Error(_), true, Authenticated(currentUser), _) =>
    <Redirect
      path={Routes.CreatorsIdAnnotationsNew.path(
        ~creatorUsername=currentUser.username,
      )}
      query={Raw.merge(
        Alert.(query_encode({alert: noDataAlert})),
        Routes.CreatorsIdAnnotationsNew.queryParams_encode({
          id: None,
          initialPhaseState: Some(`PhaseTextInput),
          creatorUsername: currentUser.username,
        }),
      )}
    />
  | (_, _, Unauthenticated, _) => <Loading />
  };
};
