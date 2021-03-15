let make = (~externalTarget=?, ~textualTarget=?, ~specificTarget=?, ()) => {
  "externalTarget": externalTarget,
  "textualTarget": textualTarget,
  "specificTarget": specificTarget,
};

let makeTextualTarget =
    (
      ~id,
      ~value,
      ~format=?,
      ~processingLanguage=?,
      ~language=?,
      ~textDirection=?,
      ~accessibility=?,
      ~rights=?,
      (),
    ) => {
  "id": id,
  "value": value,
  "format": format,
  "processingLanguage": processingLanguage,
  "language": language,
  "textDirection": textDirection,
  "accessibility": accessibility,
  "rights": rights,
};

let makeExternalTarget =
    (
      ~id,
      ~hashId,
      ~format=?,
      ~language=?,
      ~processingLanguage=?,
      ~textDirection=?,
      ~accessibility=?,
      ~rights=?,
      ~type_=?,
      (),
    ) => {
  "id": id,
  "hashId": hashId,
  "format": format,
  "processingLanguage": processingLanguage,
  "language": language,
  "textDirection": textDirection,
  "accessibility": accessibility,
  "rights": rights,
  "type": type_,
};

let makeFromTarget = target =>
  switch (target) {
  | `TextualTarget(target) =>
    make(
      ~textualTarget=
        makeTextualTarget(
          ~id=target##textualTargetId,
          ~format=?target##format,
          ~processingLanguage=?target##processingLanguage,
          ~language=?target##language,
          ~textDirection=?target##textDirection,
          ~accessibility=?target##accessibility,
          ~rights=?target##rights,
          ~value=target##value,
          (),
        ),
      (),
    )
    ->Js.Option.some
    ->Js.Promise.resolve
  | _ => Js.Promise.resolve(None)
  };

/**
 * FIXME: toTarget below requires this to keep the **input** annotation target input type polymorphic (i.e.
 * Lib_GraphQL_PatchAnnotationMutation.updateCache annotation), as unwrapped seems to constrain to just the
 * specified fields. Perhaps something with polymorphic variants being constrained on the first set of fields?
 */
external opaqueType:
  {
    ..
    "__typename": string,
    "textualTargetId": string,
    "format": option(Lib_GraphQL_Format.t),
    "processingLanguage": option(Lib_GraphQL_Language.t),
    "language": option(Lib_GraphQL_Language.t),
    "textDirection": option(Lib_GraphQL_TextDirection.t),
    "accessibility": option(array(string)),
    "rights": option(array(string)),
    "value": string,
  } =>
  Js.t('b) =
  "%identity";

let toTarget = targetInput =>
  switch (
    targetInput##textualTarget,
    targetInput##externalTarget,
    targetInput##specificTarget,
  ) {
  | (Some(textualTarget), None, None) =>
    Some(
      `TextualTarget(
        opaqueType({
          "__typename": "TextualTarget",
          "textualTargetId": textualTarget##id,
          "format": textualTarget##format,
          "processingLanguage": textualTarget##processingLanguage,
          "language": textualTarget##language,
          "textDirection": textualTarget##textDirection,
          "accessibility": textualTarget##accessibility,
          "rights": textualTarget##rights,
          "value": textualTarget##value,
        }),
      ),
    )
  | _ => None
  };

external unsafeAsDict: Js.t('a) => Js.Dict.t('b) = "%identity";

let toCache = {
  let rec textualTargetToCache = t =>
    Js.Dict.fromList([
      ("__typename", "TextualTarget"->Js.Json.string),
      ("textualTargetId", t##id->Js.Json.string),
      ("value", t##value->Js.Json.string),
      (
        "accessibility",
        t##accessibility
        ->Belt.Option.map(a =>
            a->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "rights",
        t##rights
        ->Belt.Option.map(r =>
            r->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "format",
        t##format
        ->Belt.Option.map(f => f->Lib_GraphQL_Format.toString->Js.Json.string)
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "textDirection",
        t##textDirection
        ->Belt.Option.map(t =>
            t->Lib_GraphQL_TextDirection.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "language",
        t##language
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "processingLanguage",
        t##processingLanguage
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
    ])
    ->Js.Json.object_
  and externalTargetToCache = t =>
    Js.Dict.fromList([
      ("__typename", "ExternalTarget"->Js.Json.string),
      ("externalTargetId", t##id->Js.Json.string),
      (
        "accessibility",
        t##accessibility
        ->Belt.Option.map(a =>
            a->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "rights",
        t##rights
        ->Belt.Option.map(r =>
            r->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "format",
        t##format
        ->Belt.Option.map(f => f->Lib_GraphQL_Format.toString->Js.Json.string)
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "textDirection",
        t##textDirection
        ->Belt.Option.map(t =>
            t->Lib_GraphQL_TextDirection.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "language",
        t##language
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "processingLanguage",
        t##processingLanguage
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "type_",
        unsafeAsDict(t)
        ->Js.Dict.get("type")
        ->Belt.Option.map(t =>
            t->Lib_GraphQL_ResourceType.toJs->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
    ])
    ->Js.Json.object_
  and specificTargetToCache = t =>
    Js.Dict.fromList([
      ("__typename", "SpecificTarget"->Js.Json.string),
      ("specificTargetId", t##id->Js.Json.string),
      (
        "source",
        t##source->targetToCache->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "selector",
        t##selector
        ->Belt.Array.keepMap(Lib_GraphQL_SelectorInput.toCache)
        ->Js.Json.array,
      ),
    ])
    ->Js.Json.object_
  and targetToCache = t =>
    switch (t##textualTarget, t##externalTarget, t##specificTarget) {
    | (Some(textualTarget), _, _) =>
      textualTarget->textualTargetToCache->Js.Option.some
    | (_, Some(externalTarget), _) =>
      externalTarget->externalTargetToCache->Js.Option.some
    | (_, _, Some(specificTarget)) =>
      specificTarget->specificTargetToCache->Js.Option.some
    | _ =>
      Js.Exn.raiseError(
        "AnnotationTargetInput must have at least one input set.",
      )
    };

  targetToCache;
};
