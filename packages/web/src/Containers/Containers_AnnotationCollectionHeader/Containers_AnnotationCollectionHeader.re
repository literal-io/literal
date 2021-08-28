open Containers_AnnotationCollectionHeader_GraphQL;
open Styles;

let fragmentFromCache = (~annotationCollectionIdComponent, ~identityId=?, ()) =>
  if (annotationCollectionIdComponent
      == Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent) {
    Some({
      "label": "recent",
      "id": annotationCollectionIdComponent,
      "type_": [|`TAG_COLLECTION|],
    });
  } else {
    identityId
    ->Belt.Option.flatMap(identityId =>
        Lib_GraphQL_AnnotationCollection.Apollo.readCache(
          ~id=
            Lib_GraphQL.AnnotationCollection.makeIdFromComponent(
              ~identityId,
              ~annotationCollectionIdComponent,
              (),
            ),
          ~fragment=Containers_AnnotationCollectionHeader_GraphQL.cacheAnnotationCollectionFragment,
        )
      )
    ->Belt.Option.map(result =>
        Containers_AnnotationCollectionHeader_GraphQL.GetAnnotationCollectionFragment.AnnotationCollectionHeader_AnnotationCollection.parse(
          result,
        )
      );
  };

let getTextToShare = (~format, ~annotation) => {
  let targetText =
    annotation
    ->Belt.Option.flatMap(a =>
        a##target
        ->Belt.Array.getBy(t =>
            switch (t) {
            | `TextualTarget(_) => true
            | _ => false
            }
          )
      )
    ->Belt.Option.flatMap(t =>
        switch (t) {
        | `TextualTarget(t) => Some(t##value)
        | _ => None
        }
      );

  let sourceUrl =
    annotation
    ->Belt.Option.flatMap(a => {
        let specificTarget =
          a##target
          ->Belt.Array.getBy(t =>
              switch (t) {
              | `SpecificTarget(t) => true
              | _ => false
              }
            );
        switch (specificTarget) {
        | Some(_) => specificTarget
        | None =>
          a##target
          ->Belt.Array.getBy(t =>
              switch (t) {
              | `ExternalTarget(t) => true
              | _ => false
              }
            )
        };
      })
    ->Belt.Option.flatMap(t => {
        switch (t) {
        | `SpecificTarget(st) =>
          switch (st##source) {
          | `ExternalTarget(et) => Some(et##externalTargetId)
          | _ => None
          }
        | `ExternalTarget(et) => Some(et##externalTargetId)
        | _ => None
        }
      })
    ->Belt.Option.map(url =>
        targetText
        ->Belt.Option.map(targetText => {
            let textFragment =
              if (Js.String2.length(targetText) < 300) {
                targetText->Js.Global.encodeURIComponent;
              } else {
                let encodeTextPart = (s, ~offset, ~len) => {
                  let part =
                    s
                    ->Belt.Array.slice(~offset, ~len)
                    ->Belt.Array.joinWith(" ", i => i);

                  if (Js.String2.length(part) < 150) {
                    Js.Global.encodeURIComponent(part);
                  } else {
                    part
                    ->Js.String2.slice(~from=0, ~to_=150)
                    ->Js.Global.encodeURIComponent;
                  };
                };

                let splitBody = targetText->Js.String2.split(" ");
                let textStart = encodeTextPart(splitBody, ~offset=0, ~len=5);
                let textEnd =
                  encodeTextPart(
                    splitBody,
                    ~offset=Belt.Array.length(splitBody) - 5,
                    ~len=5,
                  );
                textStart ++ "," ++ textEnd;
              };

            let u = Webapi.Url.make(url);
            Webapi.Url.setHash(u, ":~:text=" ++ textFragment);
            Webapi.Url.href(u);
          })
        ->Belt.Option.getWithDefault(url)
      );

  switch (format, targetText, sourceUrl) {
  | (`Text, Some(targetText), None) => Some(targetText)
  | (`Markdown, Some(targetText), None) => Some("> " ++ targetText)
  | (`Text, Some(targetText), Some(sourceUrl)) =>
    Some(targetText ++ "(" ++ sourceUrl ++ ")")
  | (`Markdown, Some(targetText), Some(sourceUrl)) =>
    let url = Webapi.Url.make(sourceUrl);
    let displayHost = url->Webapi.Url.host;
    let displayPath = url->Webapi.Url.pathname;
    Some(
      "> "
      ++ targetText
      ++ "\n\n"
      ++ "["
      ++ displayHost
      ++ displayPath
      ++ "]("
      ++ sourceUrl
      ++ ")",
    );
  | _ => None
  };
};

[@react.component]
let make =
    (
      ~hideDelete=false,
      ~annotationFragment as annotation=?,
      ~annotationCollectionFragment as annotationCollection=?,
      ~identityId=?,
    ) => {
  let (deleteAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_AnnotationCollectionHeader_GraphQL.DeleteAnnotationMutation.definition,
    );

  let handleDelete = () =>
    switch (annotation, identityId) {
    | (Some(annotation), Some(identityId)) =>
      let input =
        Lib_GraphQL_DeleteAnnotationMutation.Input.make(
          ~creatorUsername=identityId,
          ~id=annotation##id,
        );

      let variables = DeleteAnnotationMutation.makeVariables(~input, ());

      let _ = deleteAnnotationMutation(~variables, ());
      let _ =
        Lib_GraphQL_DeleteAnnotationMutation.Apollo.updateCache(
          ~annotation,
          ~identityId,
        );
      ();
    | _ => ()
    };

  let handleShare = (~format) => {
    let text = getTextToShare(~format, ~annotation);
    let contentType =
      switch (format) {
      | `Text => "text/plain"
      | `Markdown => "text/plain"
      };

    let handled = ref(false);
    switch (text) {
    | Some(text) =>
      handled :=
        Webview.(
          postMessage(
            WebEvent.make(
              ~type_="ACTION_SHARE",
              ~data=
                Js.Json.object_(
                  Js.Dict.fromList([
                    ("text", Js.Json.string(text)),
                    ("contentType", Js.Json.string(contentType)),
                  ]),
                ),
              (),
            ),
          )
        )
    | _ => ()
    };

    // TODO: show error on handled == false
    ();
  };

  let createButton =
    <MaterialUi.IconButton
      size=`Small
      onClick={_ => {
        let _ =
          Service_Analytics.(track(Click({action: "create", label: None})));
        ();
      }}
      edge=MaterialUi.IconButton.Edge._end
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(
        ~root=cn(["p-0", "mr-6"]),
        (),
      )}>
      <Svg
        className={cn(["pointer-events-none", "opacity-75"])}
        style={ReactDOMRe.Style.make(~width="1.75rem", ~height="1.75rem", ())}
        icon=Svg.add
      />
    </MaterialUi.IconButton>;

  <>
    <Header
      className={cn([
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "flex",
        "flex-row",
        "flex-shrink-0",
        "justify-between",
        "h-14",
        "py-2",
        "px-4",
        "max-w-full",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "items-center",
          "flex-shrink",
          "overflow-x-auto",
        ])}>
        <AnnotationCollectionHeader_Title ?annotationCollection />
      </div>
      <div className={Cn.fromList(["flex", "items-center", "flex-shrink-0", "ml-4"])}>
        {switch (identityId) {
         | Some(identityId) =>
           <Next.Link
             _as={Routes.CreatorsIdAnnotationsNew.path(~identityId)}
             href=Routes.CreatorsIdAnnotationsNew.staticPath>
             createButton
           </Next.Link>
         | _ => createButton
         }}
        <AnnotationCollectionHeader_OverflowMenu
          hideDelete
          onDelete=handleDelete
          onShare=handleShare
        />
      </div>
    </Header>
  </>;
};
