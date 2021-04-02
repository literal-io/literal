open Containers_NewAnnotationEditor_Types;

let noDataAlert = "Unable to parse image. Make sure the text is clearly highlighted and try again, or enter the text manually.";

[@bs.deriving accessors]
type action =
  | SetPhase(phase);

[@react.component]
let make = (~currentUser, ~initialPhaseState=`PhasePrompt) => {
  let (phaseState, dispatchPhaseAction) =
    React.useReducer(
      (_, action) => {
        switch (action) {
        | SetPhase(nextPhase) => nextPhase
        }
      },
      initialPhaseState,
    );
  let hasRegisteredForAddCacheAnnotation = React.useRef(false);
  React.useEffect0(() =>
    Some(
      () => {
        if (hasRegisteredForAddCacheAnnotation.current) {
          Webview.WebEventHandler.unregister("ADD_CACHE_ANNOTATIONS");
        };
        ();
      },
    )
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

  let handleCreateFromWeb = () => {
    let handleAddCacheAnnotations = ev => {
      let annotations =
        ev
        ->Belt.Option.flatMap(Js.Json.decodeObject)
        ->Belt.Option.flatMap(d => d->Js.Dict.get("annotations"))
        ->Belt.Option.flatMap(Js.Json.decodeString)
        ->Belt.Option.map(a =>
            try(a->Js.Json.parseExn) {
            | _ => Js.Json.null
            }
          )
        ->Belt.Option.flatMap(Js.Json.decodeArray)
        ->Belt.Option.map(json => {
            json->Belt.Array.keepMap(json =>
              switch (Lib_WebView_Model_Annotation.decode(json)) {
              | Ok(r) => Some(r)
              | Error(e) =>
                Js.log2("Error decoding annotation", e);
                None;
              }
            )
          })
        ->Belt.Option.getWithDefault([||]);

      if (Js.Array2.length(annotations) > 0) {
        let _ =
          Lib_WebView_Model_Apollo.addManyToCache(~annotations, ~currentUser);
        Routes.CreatorsIdAnnotationCollectionsId.(
          Next.Router.replaceWithAs(
            staticPath,
            path(
              ~creatorUsername=currentUser.username,
              ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
            ),
          )
        );
      };

      Webview.WebEventHandler.unregister("ADD_CACHE_ANNOTATIONS");
      hasRegisteredForAddCacheAnnotation.current = false;
      ();
    };

    let _ =
      Webview.WebEventHandler.register((
        "ADD_CACHE_ANNOTATIONS",
        handleAddCacheAnnotations,
      ));
    hasRegisteredForAddCacheAnnotation.current = true;

    let _ =
      Webview.(
        postMessage(
          WebEvent.make(~type_="CREATE_ANNOTATION_FROM_SOURCE", ()),
        )
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
      onCreateFromWeb=handleCreateFromWeb
    />
  };
};
