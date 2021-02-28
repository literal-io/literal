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

let toTarget = targetInput =>
  switch (
    targetInput##textualTarget,
    targetInput##externalTarget,
    targetInput##specificTarget,
  ) {
  | (Some(textualTarget), None, None) =>
    Some(
      `TextualTarget({
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
    )
  | _ => None
  };
