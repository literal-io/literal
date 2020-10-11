open Containers_AnnotationEditor_GraphQL;

let handleUpdateCache =
    (
      ~annotation,
      ~tags: array(Containers_AnnotationEditor_Types.tag),
      ~currentUser,
    ) => {
  let currentTags =
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          Some(body)
        | _ => None
        }
      );

  let addedTags =
    tags->Belt.Array.keep(tag =>
      currentTags
      ->Belt.Array.getBy(currentTag => currentTag##id == tag.id)
      ->Js.Option.isNone
    );
  let removedTags =
    currentTags->Belt.Array.keep(currentTag =>
      tags->Belt.Array.getBy(tag => tag.id == currentTag##id)->Js.Option.isNone
    );

  let _ =
    removedTags->Belt.Array.forEach(removedTag => {
      let cacheQuery =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
          ~creatorUsername=
            currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
          ~id=removedTag##id->Belt.Option.getExn,
          (),
        );
      let data =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
          ~query=cacheQuery,
          ~client=Providers_Apollo.client,
          (),
        );
      let _ =
        data
        ->Belt.Option.flatMap(d =>
            d##getAnnotationCollection->Js.Null.toOption
          )
        ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
        ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
        ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
        ->Belt.Option.forEach(items => {
            let newItems =
              items
              ->Belt.Array.keep(d => d##annotation##id != annotation##id)
              ->Js.Null.return;
            let newData =
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
                data->Js.Option.getExn,
                newItems,
              );
            let _ =
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
                ~query=cacheQuery,
                ~client=Providers_Apollo.client,
                ~data=newData,
                (),
              );
            ();
          });
      ();
    });

  let cacheAnnotation =
    QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.parsedAnnotationToCache(
      annotation,
    );
  let _ =
    addedTags->Belt.Array.forEach(addedTag => {
      let cacheQuery =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.Query.make(
          ~creatorUsername=
            currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
          ~id=addedTag.id->Belt.Option.getExn,
          (),
        );
      let data =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.readCache(
          ~query=cacheQuery,
          ~client=Providers_Apollo.client,
          (),
        );
      let newData =
        switch (data) {
        | Some(data) =>
          let items =
            data##getAnnotationCollection
            ->Js.Null.toOption
            ->Belt.Option.flatMap(d => d##first->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.flatMap(d => d##items->Js.Null.toOption)
            ->Belt.Option.getWithDefault([||]);
          let newItems =
            Js.Null.return(
              Belt.Array.concat(
                [|{"__typename": "", "annotation": cacheAnnotation}|],
                items,
              ),
            );

          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setAnnotationPageItems(
            data,
            newItems,
          );
        | None => {
            "__typename": "Query",
            "getAnnotationCollection":
              Js.Null.return({
                "__typename": "AnnotationCollection",
                "label": addedTag.text,
                "first":
                  Js.Null.return({
                    "__typename": "AnnotationPage",
                    "items":
                      Js.Null.return({
                        "__typename": "ModelAnnotationPageItemConnection",
                        "nextToken": Js.Null.return(""),
                        "items":
                          Js.Null.return([|
                            {
                              "__typename": "AnnotationPageItem",
                              "annotation": cacheAnnotation,
                            },
                          |]),
                      }),
                  }),
              }),
          }
        };
      let _ =
        QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.writeCache(
          ~query=cacheQuery,
          ~client=Providers_Apollo.client,
          ~data=newData,
          (),
        );
      ();
    });

  ();
};

let textValue = (~annotation) =>
  annotation##target
  ->Belt.Array.getBy(target =>
      switch (target) {
      | `TextualTarget(_) => true
      | `ExternalTarget(_) => false
      }
    )
  ->Belt.Option.flatMap(target =>
      switch (target) {
      | `TextualTarget(target) => Some(target##value)
      | `ExternalTarget(_) => None
      }
    )
  ->Belt.Option.getWithDefault("");

let tagsValue = (~currentUser, ~annotation) =>
  annotation##body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
          let href =
            body##id
            ->Belt.Option.map(id =>
                Lib_GraphQL.AnnotationCollection.(
                  makeIdFromComponent(
                    ~annotationCollectionIdComponent=idComponent(id),
                    ~creatorUsername=
                      currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                    ~origin=
                      Webapi.Dom.(window->Window.location->Location.origin),
                    (),
                  )
                )
              );
          Some(
            Containers_AnnotationEditor_Types.{
              text: body##value,
              id: body##id,
              href,
            },
          );
        | `Nonexhaustive => None
        | _ => None
        }
      )
    )
  ->Belt.Option.getWithDefault([|
      Lib_GraphQL.AnnotationCollection.{
        text: recentAnnotationCollectionLabel,
        id:
          makeIdFromComponent(
            ~creatorUsername=
              currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
            ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
            (),
          )
          ->Js.Option.some,
        href: None,
      },
    |]);

let handleSave =
  Lodash.debounce2(
    (.
      variables,
      updateAnnotationMutation:
        ApolloHooks.Mutation.mutation(PatchAnnotationMutation.t),
    ) => {
      let _ = updateAnnotationMutation(~variables, ());
      ();
    },
    500,
  );

[@react.component]
let make = (~annotationFragment as annotation, ~currentUser) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);

  let handleTagsChange = tagsValue => {
    let tagsWithIds =
      tagsValue
      ->Belt.Array.map((tag: Containers_AnnotationEditor_Types.tag) => {
          let id =
            switch (tag.id) {
            | Some(id) => Js.Promise.resolve(id)
            | None =>
              Lib_GraphQL.AnnotationCollection.makeId(
                ~creatorUsername=
                  AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                ~label=tag.text,
              )
            };
          id
          |> Js.Promise.then_(id =>
               Js.Promise.resolve({...tag, id: Some(id)})
             );
        })
      ->Js.Promise.all;

    let _ =
      tagsWithIds
      |> Js.Promise.then_(tags => {
           let updateBody =
             tags->Belt.Array.map(
               (tag: Containers_AnnotationEditor_Types.tag) =>
               {
                 "textualBody":
                   Some({
                     "id": tag.id,
                     "value": tag.text,
                     "purpose": Some([|`TAGGING|]),
                     "rights": None,
                     "accessibility": None,
                     "format": Some(`TEXT_PLAIN),
                     "textDirection": Some(`LTR),
                     "language": Some(`EN_US),
                     "processingLanguage": Some(`EN_US),
                     "type": Some(`TEXTUAL_BODY),
                   }),
                 "externalBody": None,
                 "choiceBody": None,
                 "specificBody": None,
               }
             );
           let variables =
             PatchAnnotationMutation.makeVariables(
               ~input={
                 "id": annotation##id,
                 "creatorUsername":
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 "operations": [|
                   {
                     "set": Some({"body": Some(updateBody), "target": None}),
                   },
                 |],
               },
               (),
             );
           let _ = handleSave(. variables, patchAnnotationMutation);
           Js.Promise.resolve();
         });
    let _ =
      tagsWithIds
      |> Js.Promise.then_(tags => {
           let _ = handleUpdateCache(~annotation, ~tags, ~currentUser);
           Js.Promise.resolve();
         });
    ();
  };

  let handleTextChange = textValue => {
    let updateTargetInput = {
      let idx =
        annotation##target
        ->Belt.Array.getIndexBy(target =>
            switch (target) {
            | `TextualTarget(_) => true
            | `ExternalTarget(_) => false
            }
          );
      let updatedTextualTarget =
        idx
        ->Belt.Option.flatMap(idx => annotation##target->Belt.Array.get(idx))
        ->Belt.Option.flatMap(target =>
            switch (target) {
            | `TextualTarget(target) =>
              let copy = Js.Obj.assign(Js.Obj.empty(), target);
              Some(
                `TextualTarget(Js.Obj.assign(copy, {"value": textValue})),
              );
            | `ExternalTarget(_) => None
            }
          )
        ->Belt.Option.getWithDefault(
            `TextualTarget({
              "__typename": "TextualTarget",
              "textualTargetId": None,
              "format": Some(`TEXT_PLAIN),
              "language": Some(`EN_US),
              "processingLanguage": Some(`EN_US),
              "textDirection": Some(`LTR),
              "accessibility": None,
              "rights": None,
              "value": textValue,
            }),
          );

      let updatedTarget = Belt.Array.copy(annotation##target);
      let _ =
        switch (idx) {
        | Some(idx) =>
          let _ = updatedTarget->Belt.Array.set(idx, updatedTextualTarget);
          ();
        | None =>
          let _ = updatedTarget->Js.Array2.push(updatedTextualTarget);
          ();
        };

      updatedTarget->Belt.Array.map(
        Lib_GraphQL.Annotation.targetInputFromTarget,
      );
    };

    let variables =
      PatchAnnotationMutation.makeVariables(
        ~input={
          "id": annotation##id,
          "creatorUsername":
            AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          "operations": [|
            {
              "set": Some({"body": None, "target": Some(updateTargetInput)}),
            },
          |],
        },
        (),
      );
    let _ = handleSave(. variables, patchAnnotationMutation);
    ();
  };

  let _ =
    React.useEffect0(() => {
      Some(
        () => {
          let _ = Lodash.flush2(handleSave);
          ();
        },
      )
    });

  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={Cn.fromList(["px-6", "pb-4", "pt-16"])}>
      <TextInput.Annotation
        onTextChange=handleTextChange
        onTagsChange=handleTagsChange
        textValue={textValue(~annotation)}
        tagsValue={tagsValue(~annotation, ~currentUser)}
      />
    </div>
  </div>;
};
