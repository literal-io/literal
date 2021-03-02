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
  open QueryRenderers_AnnotationCollection_GraphQL;
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
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.getIndexBy(body =>
        switch (body) {
        | `TextualBody(body) => body##id == clause##id
        | _ => false
        }
      );

  let getAnnotationTargetIndexByWhere = (annotation, clause) =>
    annotation##target
    ->Belt.Array.getIndexBy(target =>
        switch (target) {
        | `TextualTarget(target) => target##textualTargetId == clause##id
        | _ => false
        }
      );

  let getAnnotationCollectionOperations =
      (~annotation, ~operations): array(Operation.t(string, string, string)) =>
    annotation##body
    ->Belt.Option.getWithDefault([||])
    ->Belt.Array.keepMap(body =>
        switch (body) {
        | `TextualBody(body) =>
          if (Lib_GraphQL.Annotation.isBodyTag(body)) {
            Some(body);
          } else {
            None;
          }
        | _ => None
        }
      )
    ->Belt.Array.map(textualBody =>
        operations
        ->Belt.Array.getBy(op =>
            switch (op) {
            | Operation.(Some(Remove(remove))) =>
              remove##where##id == textualBody##id
            | _ => false
            }
          )
        ->Belt.Option.flatMap(op => {
            switch (op) {
            | Some(Remove(remove)) =>
              Operation.(Some(Remove(textualBody##id)))
            | _ => None
            }
          })
        ->Belt.Option.getWithDefault(Set(textualBody##id))
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
                let id =
                  body##textualBody->Belt.Option.getExn->(body => body##id);
                Operation.(Some(Add(id)));
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
      op##body->Belt.Option.flatMap(Lib_GraphQL_AnnotationBodyInput.toBody),
      op##target
      ->Belt.Option.flatMap(Lib_GraphQL_AnnotationTargetInput.toTarget),
    ) {
    | (Some(body), None) =>
      Ramda.assoc(
        annotation,
        "body",
        Belt.Array.concat(
          annotation##body->Belt.Option.getWithDefault([||]),
          [|body|],
        ),
      )
    | (None, Some(target)) =>
      Ramda.assoc(
        annotation,
        "target",
        Belt.Array.concat(annotation##target, [|target|]),
      )
    | _ => annotation
    };
  };

  let applySetOperation = (~op, ~annotation) =>
    switch (
      op##body->Belt.Option.flatMap(Lib_GraphQL_AnnotationBodyInput.toBody),
      op##target
      ->Belt.Option.flatMap(Lib_GraphQL_AnnotationTargetInput.toTarget),
    ) {
    | (Some(body), None) =>
      annotation
      ->getAnnotationBodyIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation##body
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
          let copy = annotation##target->Belt.Array.copy;
          let _ = Belt.Array.set(copy, idx, target);
          Ramda.assoc(annotation, "target", copy);
        })
      ->Belt.Option.getWithDefault(annotation)

    | _ => annotation
    };

  let applyRemoveOperation = (~op, ~annotation) => {
    switch (op##body, op##target) {
    | (Some(true), _) =>
      annotation
      ->getAnnotationBodyIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy =
            annotation##body
            ->Belt.Option.getWithDefault([||])
            ->Belt.Array.copy;
          let _ =
            Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1, ~add=[||]);
          Ramda.assoc(annotation, "body", copy);
        })
      ->Belt.Option.getWithDefault(annotation)
    | (_, Some(true)) =>
      annotation
      ->getAnnotationTargetIndexByWhere(op##where)
      ->Belt.Option.map(idx => {
          let copy = annotation##target->Belt.Array.copy;
          let _ = Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1);
          Ramda.assoc(annotation, "target", copy);
        })
      ->Belt.Option.getWithDefault(annotation)
    | _ => annotation
    };
  };

  let updateCache = (~annotation, ~currentUser, ~input) => {
    let parsedOperations = input##operations->Belt.Array.map(Operation.parse);

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
                getAnnotationBodyIndexByWhere(annotation, remove1##where),
                getAnnotationBodyIndexByWhere(annotation, remove2##where),
              ) {
              | (Some(idx1), Some(idx2)) => idx2 - idx1
              | (Some(_), _) => (-1)
              | (_, Some(_)) => 1
              | _ => 0
              }
            | (Some(Target(_)), Some(Target(_))) =>
              switch (
                getAnnotationTargetIndexByWhere(annotation, remove1##where),
                getAnnotationTargetIndexByWhere(annotation, remove2##where),
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
      ->Belt.Array.reduce(annotation, (annotation, operation) =>
          switch (operation) {
          | Some(Add(add)) => applyAddOperation(~op=add, ~annotation)
          | Some(Set(set)) => applySetOperation(~op=set, ~annotation)
          | Some(Remove(remove)) =>
            applyRemoveOperation(~op=remove, ~annotation)
          | _ => annotation
          }
        )
      ->Ramda.assoc(
          "modified",
          Js.Date.(make()->toISOString)->Js.Json.string,
        );

    let cacheAnnotation =
      GetAnnotationCollection.parsedAnnotationToCache(updatedAnnotation);
    let onCreateAnnotationCollection = () => None;

    let _ =
      getAnnotationCollectionOperations(
        ~annotation,
        ~operations=parsedOperations,
      )
      ->Belt.Array.forEach(op =>
          switch (op) {
          | Add(annotationCollectionId) =>
            Lib_GraphQL_AnnotationCollection.Apollo.addAnnotationToCollection(
              ~annotation=cacheAnnotation,
              ~currentUser,
              ~annotationCollectionId,
              ~onCreateAnnotationCollection,
            )
          | Remove(annotationCollectionId) =>
            Lib_GraphQL_AnnotationCollection.Apollo.removeAnnotationFromCollection(
              ~annotationId=cacheAnnotation##id,
              ~currentUser,
              ~annotationCollectionId,
            )
          | Set(annotationCollectionId) =>
            Lib_GraphQL_AnnotationCollection.Apollo.setAnnotationInCollection(
              ~annotation=cacheAnnotation,
              ~currentUser,
              ~annotationCollectionId,
            )
          }
        );
    ();
  };
};
