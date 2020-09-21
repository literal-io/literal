open Containers_AnnotationEditor_Annotation_GraphQL;

let handleUpdateCache =
    (
      ~annotation,
      ~tags: array(Containers_AnnotationEditor_Base_Types.tag),
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
        ->Belt.Option.flatMap(d => d##getAnnotationCollection)
        ->Belt.Option.flatMap(d => d##first)
        ->Belt.Option.flatMap(d => d##items)
        ->Belt.Option.flatMap(d => d##items)
        ->Belt.Option.forEach(items => {
            let newItems =
              items->Belt.Array.keep(d => d##annotation##id != annotation##id);
            let newData =
              QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setCacheItems(
                data,
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

  let cacheAnnotation = {
    "__typename": "Annotation",
    "created": annotation##created,
    "id": annotation##id,
    "body":
      annotation##body
      ->Belt.Option.map(d =>
          d
          ->Belt.Array.keepMap(d =>
              switch (d) {
              | `TextualBody(d) => Some(d)
              | `Nonexhaustive => None
              }
            )
          ->Belt.Array.map(d =>
              {
                "__typename": "TextualBody",
                "id": d##id->Js.Null.fromOption,
                "value": d##value,
                "purpose":
                  d##purpose
                  ->Belt.Option.map(d =>
                      d->Belt.Array.map(d =>
                        switch (d) {
                        | `TAGGING => "TAGGING"
                        | `ACCESSING => "ACCESSING"
                        | `BOOKMARKING => "BOOKMARKING"
                        | `CLASSIFYING => "CLASSIFYING"
                        | `COMMENTING => "COMMENTING"
                        | `DESCRIBING => "DESCRIBING"
                        | `EDITING => "EDITING"
                        | `HIGHLIGHTING => "HIGHLIGHTING"
                        | `IDENTIFYING => "IDENTIFYING"
                        | `LINKING => "LINKING"
                        | `MODERATING => "MODERATING"
                        | `QUESTIONING => "QUESTIONING"
                        | `REPLYING => "REPLYING"
                        }
                      )
                    )
                  ->Js.Null.fromOption,
                "format":
                  d##format
                  ->Belt.Option.map(d =>
                      switch (d) {
                      | `TEXT_PLAIN => "TEXT_PLAIN"
                      }
                    )
                  ->Js.Null.fromOption,
                "language":
                  d##language
                  ->Belt.Option.map(d =>
                      switch (d) {
                      | `EN_US => "EN_US"
                      }
                    )
                  ->Js.Null.fromOption,
                "processingLanguage":
                  d##processingLanguage
                  ->Belt.Option.map(d =>
                      switch (d) {
                      | `EN_US => "EN_US"
                      }
                    )
                  ->Js.Null.fromOption,
                "accessibility": d##accessibility->Js.Null.fromOption,
                "rights": d##rights->Js.Null.fromOption,
                "textDirection":
                  d##textDirection
                  ->Belt.Option.map(d =>
                      switch (d) {
                      | `LTR => "LTR"
                      | `RTL => "RTL"
                      | `AUTO => "AUTO"
                      }
                    )
                  ->Js.Null.fromOption,
              }
            )
        ),
    "target":
      annotation##target
      ->Belt.Array.map(d =>
          switch (d) {
          | `TextualTarget(d) => {
              "__typename": "TextualTarget",
              "value": Js.Null.return(d##value),
              "textualTargetId": d##textualTargetId->Js.Null.fromOption,
              "externalTargetId": Js.Null.empty,
              "format":
                d##format
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `TEXT_PLAIN => "TEXT_PLAIN"
                    }
                  )
                ->Js.Null.fromOption,
              "language":
                d##language
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `EN_US => "EN_US"
                    }
                  )
                ->Js.Null.fromOption,
              "processingLanguage":
                d##processingLanguage
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `EN_US => "EN_US"
                    }
                  )
                ->Js.Null.fromOption,
              "accessibility": d##accessibility->Js.Null.fromOption,
              "rights": d##rights->Js.Null.fromOption,
              "textDirection":
                d##textDirection
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `LTR => "LTR"
                    | `RTL => "RTL"
                    | `AUTO => "AUTO"
                    }
                  )
                ->Js.Null.fromOption,
            }
          | `ExternalTarget(d) => {
              "__typename": "ExternalTarget",
              "value": Js.Null.empty,
              "textualTargetId": Js.Null.empty,
              "externalTargetId": d##externalTargetId->Js.Null.return,
              "format":
                d##format
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `TEXT_PLAIN => "TEXT_PLAIN"
                    }
                  )
                ->Js.Null.fromOption,
              "language":
                d##language
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `EN_US => "EN_US"
                    }
                  )
                ->Js.Null.fromOption,
              "processingLanguage":
                d##processingLanguage
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `EN_US => "EN_US"
                    }
                  )
                ->Js.Null.fromOption,
              "accessibility": d##accessibility->Js.Null.fromOption,
              "rights": d##rights->Js.Null.fromOption,
              "textDirection":
                d##textDirection
                ->Belt.Option.map(d =>
                    switch (d) {
                    | `LTR => "LTR"
                    | `RTL => "RTL"
                    | `AUTO => "AUTO"
                    }
                  )
                ->Js.Null.fromOption,
            }
          }
        ),
  };

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
            ->Belt.Option.flatMap(d => d##first)
            ->Belt.Option.flatMap(d => d##items)
            ->Belt.Option.flatMap(d => d##items)
            ->Belt.Option.getWithDefault([||]);
          let newItems =
            Belt.Array.concat(
              [|{"__typename": "", "annotation": cacheAnnotation}|],
              items,
            );

          QueryRenderers_AnnotationCollection_GraphQL.GetAnnotationCollection.setCacheItems(
            data,
            newItems,
          );
        | None => {
            "__typename": "Query",
            "getAnnotationCollection":
              Some({
                "__typename": "AnnotationCollection",
                "label": addedTag.text,
                "first":
                  Some({
                    "__typename": "AnnotationPage",
                    "items":
                      Some({
                        "__typename": "ModelAnnotationPageItemConnection",
                        "items":
                          Some([|
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
let make = (~annotationFragment as annotation, ~isActive, ~currentUser) => {
  let (patchAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(PatchAnnotationMutation.definition);

  let handleChange =
      (editorValue: Containers_AnnotationEditor_Base_Types.value) => {
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
                `TextualTarget(
                  Js.Obj.assign(copy, {"value": editorValue.text}),
                ),
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
              "value": editorValue.text,
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

    let tagsWithIds =
      editorValue.tags
      ->Belt.Array.map(tag => {
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

    let updateBodyInput = {
      tagsWithIds
      |> Js.Promise.then_(tags =>
           tags
           ->Belt.Array.map((tag: Containers_AnnotationEditor_Base_Types.tag) =>
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
             )
           ->Js.Promise.resolve
         );
    };

    let _ =
      updateBodyInput
      |> Js.Promise.then_(updateBodyInput => {
           let variables =
             PatchAnnotationMutation.makeVariables(
               ~input={
                 "id": annotation##id,
                 "creatorUsername":
                   AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                 "operations": [|
                   {
                     "set":
                       Some({"body": Some(updateBodyInput), "target": None}),
                   },
                   {
                     "set":
                       Some({
                         "body": None,
                         "target": Some(updateTargetInput),
                       }),
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
           let _ = handleUpdateCache(~annotation, ~currentUser, ~tags);
           Js.Promise.resolve();
         });
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
  <Containers_AnnotationEditor_Base
    annotationFragment=annotation
    isActive
    onChange=handleChange
    currentUser
  />;
};
