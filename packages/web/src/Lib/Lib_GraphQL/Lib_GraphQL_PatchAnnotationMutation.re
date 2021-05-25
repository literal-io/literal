module Input = {
  let make = (~id, ~creatorUsername, ~operations) => {
    "id": id,
    "creatorUsername": creatorUsername,
    "operations": operations,
  };
  let makeOperation = (~add=?, ~remove=?, ~set=?, ()) => {
    "add": add,
    "remove": remove,
    "set": set,
  };

  let makeAdd = (~target=?, ~body=?, ()) => {"target": target, "body": body};

  let makeRemove = (~where_, ~target=?, ~body=?, ()) => {
    "target": target,
    "body": body,
    "where": where_,
  };

  let makeSet = (~where_, ~target=?, ~body=?, ()) => {
    "where": where_,
    "target": target,
    "body": body,
  };

  let makeWhere = (~id, ()) => {"id": id};

  /** compute the set of operations required to transform old body to new body **/
  let makeFromBodyDiff = (~oldBody, ~newBody) => {
    let withId = bodies =>
      bodies->Belt.Array.keepMap(body => {
        let id =
          switch (body) {
          | `TextualBody(textualBody) => Some(textualBody##id)
          | _ => None
          };

        id->Belt.Option.map(id => (id, body));
      });

    let newBodyWithId = withId(newBody);
    let oldBodyWithId = withId(oldBody);

    let removeOperations =
      oldBodyWithId
      ->Belt.Array.keep(((oldId, _)) =>
          !newBodyWithId->Belt.Array.some(((newId, _)) => oldId == newId)
        )
      ->Belt.Array.map(((id, _)) =>
          makeOperation(
            ~remove=makeRemove(~body=true, ~where_=makeWhere(~id, ()), ()),
            (),
          )
        );
    let addOperations =
      newBodyWithId
      ->Belt.Array.keep(((newId, _)) =>
          !oldBodyWithId->Belt.Array.some(((oldId, _)) => oldId == newId)
        )
      ->Belt.Array.keepMap(((_, newBody)) => {
          newBody
          ->Lib_GraphQL_AnnotationBodyInput.makeFromBody
          ->Belt.Option.map(body =>
              makeOperation(~add=makeAdd(~body, ()), ())
            )
        });
    Belt.Array.concat(removeOperations, addOperations);
  };
};

module Apollo = {
  module Operation = {
    type t('a, 'b, 'c) =
      | Add('a)
      | Set('b)
      | Remove('c);

    let parse = input =>
      switch (input##add, input##set, input##remove) {
      | (Some(add), _, _) => Some(Add(add))
      | (_, Some(set), _) => Some(Set(set))
      | (_, _, Some(remove)) => Some(Remove(remove))
      | _ => None
      };
  };

  module OperationType = {
    type t('a, 'b) =
      | Body('a)
      | Target('b);

    let parse = input =>
      switch (input##body, input##target) {
      | (Some(body), _) => Some(Body(body))
      | (_, Some(target)) => Some(Target(target))
      | _ => None
      };
  };

  let parseBodyInputToBody = bodyInput =>
    switch (
      bodyInput##textualBody,
      bodyInput##externalBody,
      bodyInput##choiceBody,
      bodyInput##specificBody,
    ) {
    | (Some(textualBody), None, None, None) => Some(textualBody)
    | _ => None
    };

  let parseTargetInputToTarget = targetInput =>
    switch (
      targetInput##textualTarget,
      targetInput##externalTarget,
      targetInput##specificTarget,
    ) {
    | (Some(textualTarget), None, None) => Some(textualTarget)
    | _ => None
    };

  let getAnnotationBodyIndexByWhere = (annotation, clause) =>
    annotation
    ->Js.Json.decodeObject
    ->Belt.Option.flatMap(dict => dict->Js.Dict.get("body"))
    ->Belt.Option.flatMap(Js.Json.decodeArray)
    ->Belt.Option.flatMap(arr =>
        arr->Belt.Array.getIndexBy(body =>
          body
          ->Js.Json.decodeObject
          ->Belt.Option.map(body => {
              switch (
                body
                ->Js.Dict.get("__typename")
                ->Belt.Option.flatMap(Js.Json.decodeString),
                body
                ->Js.Dict.get("id")
                ->Belt.Option.flatMap(Js.Json.decodeString),
              ) {
              | (Some("TextualBody"), Some(id)) when id == clause##id => true
              | _ => false
              }
            })
          ->Belt.Option.getWithDefault(false)
        )
      );

  let getAnnotationTargetIndexByWhere = (annotation, clause) =>
    annotation
    ->Js.Json.decodeObject
    ->Belt.Option.flatMap(dict => dict->Js.Dict.get("target"))
    ->Belt.Option.flatMap(Js.Json.decodeArray)
    ->Belt.Option.flatMap(arr =>
        arr->Belt.Array.getIndexBy(body =>
          body
          ->Js.Json.decodeObject
          ->Belt.Option.map(body => {
              switch (
                body
                ->Js.Dict.get("textualTargetId")
                ->Belt.Option.flatMap(Js.Json.decodeString),
                body
                ->Js.Dict.get("specificTargetId")
                ->Belt.Option.flatMap(Js.Json.decodeString),
                body
                ->Js.Dict.get("externalTargetId")
                ->Belt.Option.flatMap(Js.Json.decodeString),
              ) {
              | (Some(id), _, _)
              | (_, Some(id), _)
              | (_, _, Some(id)) when id == clause##id => true
              | _ => false
              }
            })
          ->Belt.Option.getWithDefault(false)
        )
      );

  let getAnnotationCollectionOperations = (~annotation, ~operations) =>
    annotation
    ->Js.Json.decodeObject
    ->Belt.Option.flatMap(dict => dict->Js.Dict.get("body"))
    ->Belt.Option.flatMap(Js.Json.decodeArray)
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        body
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(body =>
            switch (
              body
              ->Js.Dict.get("__typename")
              ->Belt.Option.flatMap(Js.Json.decodeString),
              body
              ->Js.Dict.get("purpose")
              ->Belt.Option.flatMap(Js.Json.decodeArray)
              ->Belt.Option.map(p =>
                  p->Belt.Array.some(p =>
                    p
                    ->Js.Json.decodeString
                    ->Belt.Option.map(s => s == "TAGGING")
                    ->Belt.Option.getWithDefault(false)
                  )
                )
              ->Belt.Option.getWithDefault(false),
            ) {
            | (Some("TextualBody"), true) =>
              body
              ->Js.Dict.get("id")
              ->Belt.Option.flatMap(Js.Json.decodeString)
            | _ => None
            }
          )
      )
    ->Belt.Array.map(textualBodyId =>
        operations
        ->Belt.Array.getBy(op =>
            switch (op) {
            | Operation.(Some(Remove(remove))) =>
              remove##where##id == textualBodyId
            | _ => false
            }
          )
        ->Belt.Option.flatMap(op => {
            switch (op) {
            | Some(Remove(_)) => Operation.(Some(Remove(textualBodyId)))
            | _ => None
            }
          })
        ->Belt.Option.getWithDefault(Set(textualBodyId))
      )
    ->Belt.Array.concat(
        operations->Belt.Array.keepMap(op =>
          switch (op) {
          | Some(Add(add)) =>
            switch (OperationType.parse(add)) {
            | Some(Body(body)) =>
              if (body
                  ->Lib_GraphQL_AnnotationBodyInput.toBody
                  ->Belt.Option.map(body =>
                      switch (body) {
                      | `TextualBody(body) =>
                        Lib_GraphQL.Annotation.isBodyTag(body)
                      | _ => false
                      }
                    )
                  ->Belt.Option.getWithDefault(false)) {
                let (id, value) =
                  body##textualBody
                  ->Belt.Option.getExn
                  ->(body => (body##id, body##value));
                Operation.(Some(Add((id, value))));
              } else {
                None;
              }
            | _ => None
            }
          | _ => None
          }
        ),
      );

  let applyAddOperation = (~op, ~annotation) => {
    switch (
      op##body->Belt.Option.flatMap(Lib_GraphQL_AnnotationBodyInput.toCache),
      op##target
      ->Belt.Option.flatMap(Lib_GraphQL_AnnotationTargetInput.toCache),
    ) {
    | (Some(body), None) =>
      Ramda.assoc(
        annotation,
        "body",
        annotation
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(dict => dict->Js.Dict.get("body"))
        ->Belt.Option.flatMap(Js.Json.decodeArray)
        ->Belt.Option.getWithDefault([||])
        ->Belt.Array.concat([|body|]),
      )
    | (None, Some(target)) =>
      Ramda.assoc(
        annotation,
        "target",
        annotation
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(dict => dict->Js.Dict.get("target"))
        ->Belt.Option.flatMap(Js.Json.decodeArray)
        ->Belt.Option.getWithDefault([||])
        ->Belt.Array.concat([|target|]),
      )
    | _ => annotation
    };
  };

  let applySetOperation = (~op, ~annotation) =>
    switch (
      op##body->Belt.Option.flatMap(Lib_GraphQL_AnnotationBodyInput.toCache),
      op##target
      ->Belt.Option.flatMap(Lib_GraphQL_AnnotationTargetInput.toCache),
    ) {
    | (Some(body), None) =>
      annotation
      ->getAnnotationBodyIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(dict => dict->Js.Dict.get("body"))
            ->Belt.Option.flatMap(Js.Json.decodeArray)
            ->Belt.Option.getWithDefault([||])
            ->Belt.Array.copy;

          let _ = Belt.Array.set(copy, idx, body);
          Ramda.assoc(annotation, "body", copy);
        })
      ->Belt.Option.getWithDefault(annotation)
    | (None, Some(target)) =>
      annotation
      ->getAnnotationTargetIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(dict => dict->Js.Dict.get("target"))
            ->Belt.Option.flatMap(Js.Json.decodeArray)
            ->Belt.Option.getWithDefault([||])
            ->Belt.Array.copy;

          let _ = Belt.Array.set(copy, idx, target);
          Ramda.assoc(annotation, "target", copy);
        })
      ->Belt.Option.getWithDefault(annotation)

    | _ => annotation
    };

  let applyRemoveOperation = (~op, ~annotation) => {
    switch (op##body, op##target) {
    | (Some(_), _) =>
      annotation
      ->getAnnotationBodyIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(dict => dict->Js.Dict.get("body"))
            ->Belt.Option.flatMap(Js.Json.decodeArray)
            ->Belt.Option.getWithDefault([||])
            ->Belt.Array.copy;

          let _ =
            Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1, ~add=[||]);
          Ramda.assoc(annotation, "body", copy);
        })
      ->Belt.Option.getWithDefault(annotation)
    | (_, Some(_)) =>
      annotation
      ->getAnnotationTargetIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation
            ->Js.Json.decodeObject
            ->Belt.Option.flatMap(dict => dict->Js.Dict.get("target"))
            ->Belt.Option.flatMap(Js.Json.decodeArray)
            ->Belt.Option.getWithDefault([||])
            ->Belt.Array.copy;

          let _ = Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1);
          Ramda.assoc(annotation, "target", copy);
        })
      ->Belt.Option.getWithDefault(annotation)
    | _ => annotation
    };
  };

  let updateCache = (~identityId, ~input) => {
    let parsedOperations = input##operations->Belt.Array.map(Operation.parse);
    let cacheAnnotation =
      Lib_GraphQL_Annotation.readCache(input##id)->Belt.Option.getExn;

    let updatedAnnotation =
      parsedOperations
      ->Ramda.sort((. op1, op2) => {
          switch (op1, op2) {
          | (Some(Set(_)), Some(Set(_))) => 0
          | (Some(Set(_)), _) => (-1)
          | (_, Some(Set(_))) => 1
          | (Some(Add(_)), Some(Add(_))) => 0
          | (Some(Add(_)), _) => (-1)
          | (_, Some(Add(_))) => 1
          | (Some(Remove(remove1)), Some(Remove(remove2))) =>
            switch (
              OperationType.parse(remove1),
              OperationType.parse(remove2),
            ) {
            | (Some(Body(_)), Some(Body(_))) =>
              switch (
                getAnnotationBodyIndexByWhere(
                  cacheAnnotation,
                  remove1##where,
                ),
                getAnnotationBodyIndexByWhere(
                  cacheAnnotation,
                  remove2##where,
                ),
              ) {
              | (Some(idx1), Some(idx2)) => idx2 - idx1
              | (Some(_), _) => (-1)
              | (_, Some(_)) => 1
              | _ => 0
              }
            | (Some(Target(_)), Some(Target(_))) =>
              switch (
                getAnnotationTargetIndexByWhere(
                  cacheAnnotation,
                  remove1##where,
                ),
                getAnnotationTargetIndexByWhere(
                  cacheAnnotation,
                  remove1##where,
                ),
              ) {
              | (Some(idx1), Some(idx2)) => idx2 - idx1
              | (Some(_), _) => (-1)
              | (_, Some(_)) => 1
              | _ => 0
              }
            | _ => 0
            }
          | (Some(Remove(_)), _) => 1
          | (_, Some(Remove(_))) => (-1)
          | _ => 0
          }
        })
      ->Belt.Array.reduce(cacheAnnotation, (cacheAnnotation, operation) =>
          switch (operation) {
          | Some(Add(add)) =>
            applyAddOperation(~op=add, ~annotation=cacheAnnotation)
          | Some(Set(set)) =>
            applySetOperation(~op=set, ~annotation=cacheAnnotation)
          | Some(Remove(remove)) =>
            applyRemoveOperation(~op=remove, ~annotation=cacheAnnotation)
          | _ => cacheAnnotation
          }
        )
      ->Ramda.assoc(
          "modified",
          Js.Date.(make()->toISOString)->Js.Json.string,
        );

    let onCreateAnnotationCollection = () => Js.Promise.resolve(None);

    let _ =
      getAnnotationCollectionOperations(
        ~annotation=cacheAnnotation,
        ~operations=parsedOperations,
      )
      ->Belt.Array.forEach(op =>
          switch (op) {
          | Add((annotationCollectionId, annotationCollectionLabel)) =>
            Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationToCollection(
              ~annotation=updatedAnnotation,
              ~identityId,
              ~annotationCollectionId,
              ~annotationCollectionLabel,
              ~annotationCollectionType="TAG_COLLECTION",
              ~onCreateAnnotationCollection,
            )
          | Remove(annotationCollectionId) =>
            Lib_GraphQL_AnnotationCollection.Apollo.removeAnnotationFromCollection(
              ~annotationId=input##id,
              ~identityId,
              ~annotationCollectionId,
            )
          | Set(annotationCollectionId) =>
            Lib_GraphQL_AnnotationCollection.Apollo.setAnnotationInCollection(
              ~annotation=updatedAnnotation,
              ~identityId,
              ~annotationCollectionId,
            )
          }
        );
    ();
  };
};
