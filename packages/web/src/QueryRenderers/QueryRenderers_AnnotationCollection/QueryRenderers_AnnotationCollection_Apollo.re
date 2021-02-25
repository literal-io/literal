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
      | `TextualTarget(target) => target##id == clause##id
      | _ => false
      }
    );

let applyAddOperation = (~op, ~annotation) => {
  switch (
    op##body->Belt.Option.flatMap(parseBodyInputToBody),
    op##target->Belt.Option.flatMap(parseTargetInputToTarget),
  ) {
  | (Some(body), None) =>
    Ramda.assoc(
      "body",
      Belt.Array.concat(
        annotation##body->Belt.Option.getWithDefault([||]),
        body,
      ),
      annotation,
    )
  | (None, Some(target)) =>
    Ramda.assoc(
      "target",
      Belt.Array.concat(annotation##target, target),
      annotation,
    )
  | _ => annotation
  };
};

let applySetOperation = (~op, ~annotation) =>
  switch (
    op##body->Belt.Option.flatMap(parseBodyInputToBody),
    op##target->Belt.Option.flatMap(parseTargetInputToTarget),
  ) {
  | (Some(body), None) =>
    annotation
    ->getAnnotationBodyIndexByWhere(op##where)
    ->Belt.Option.map(idx => {
        let copy =
          annotation##body->Belt.Option.getWithDefault([||])->Belt.Array.copy;
        let _ = Belt.Array.set(copy, idx, body);
        Ramda.assoc("body", copy, annotation);
      })
    ->Belt.Option.getWithDefault(annotation)
  | (None, Some(target)) =>
    annotation
    ->getAnnotationTargetIndexByWhere(op##where)
    ->Belt.Option.map(idx => {
        let copy = annotation##target->Belt.Array.copy;
        let _ = Belt.Array.set(copy, idx, target);
        Ramda.assoc("target", copy, annotation);
      })
    ->Belt.Option.getWithDefault(annotation)

  | _ => annotation
  };

let applyRemoveOperation = (~op, ~annotation) =>
  switch (op##body, op##target) {
  | (Some(true), _) =>
    annotation
    ->getAnnotationBodyIndexByWhere(op##where)
    ->Belt.Option.map(idx => {
        let copy =
          annotation##body->Belt.Option.getWithDefault([||])->Belt.Array.copy;
        let _ = Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1);
        Ramda.assoc("body", copy, annotation);
      })
    ->Belt.Option.getWithDefault(annotation)
  | (_, Some(true)) =>
    annotation
    ->getAnnotationTargetIndexByWhere(op##where)
    ->Belt.Option.map(idx => {
        let copy = annotation##target->Belt.Array.copy;
        let _ = Js.Array2.spliceInPlace(copy, ~pos=idx, ~remove=1);
        Ramda.assoc("target", copy, annotation);
      })
    ->Belt.Option.getWithDefault(annotation)
  | _ => annotation
  };

let updateCacheForPatchAnnotationMutation =
    (~annotation, ~currentUser, ~input) => {
  let cacheAnnotation =
    GetAnnotationCollection.parsedAnnotationToCache(annotation);

  let updatedAnnotation =
    (
      input##operations->Belt.Array.map(Operation.parse)
      |> Ramda.sort((. op1: option(Operation.t('a, 'b, 'c)), op2) => {
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
             | (Some(Body(body1)), Some(Body(body2))) => 0
             | (Some(Target(target1)), Some(Target(target2))) => 0
             | _ => 0
             }
           }
         })
    )
    ->Belt.Array.reduce(annotation, (annotation, operation) =>
        switch (operation##add, operation##set, operation##remove) {
        | (Some(add), None, None) => applyAddOperation(~op=add, ~annotation)
        | (None, Some(set), None) => applySetOperation(~op=set, ~annotation)
        | (None, None, Some(remove)) =>
          applyRemoveOperation(~op=remove, ~annotation)
        | _ => annotation
        }
      );

  ();
};
