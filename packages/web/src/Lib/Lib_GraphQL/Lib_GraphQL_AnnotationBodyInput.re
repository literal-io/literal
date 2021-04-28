let makeBody =
    (~textualBody=?, ~externalBody=?, ~choiceBody=?, ~specificBody=?, ()) => {
  "textualBody": textualBody,
  "externalBody": externalBody,
  "choiceBody": choiceBody,
  "specificBody": specificBody,
};

let makeTextualBody =
    (
      ~id,
      ~value,
      ~purpose=?,
      ~rights=?,
      ~accessibility=?,
      ~format=?,
      ~textDirection=?,
      ~language=?,
      ~processingLanguage=?,
      (),
    ) => {
  "id": id,
  "value": value,
  "purpose": purpose,
  "rights": rights,
  "accessibility": accessibility,
  "format": format,
  "textDirection": textDirection,
  "language": language,
  "processingLanguage": processingLanguage,
  "type": Some(`TEXTUAL_BODY),
};

let makeFromBody = body =>
  switch (body) {
  | `TextualBody(textualBody) =>
    Some(
      makeBody(
        ~textualBody=
          makeTextualBody(
            ~id=textualBody##id,
            ~value=textualBody##value,
            ~purpose=?textualBody##purpose,
            ~rights=?textualBody##rights,
            ~accessibility=?textualBody##accessibility,
            ~format=?textualBody##format,
            ~textDirection=?textualBody##textDirection,
            ~language=?textualBody##language,
            ~processingLanguage=?textualBody##processingLanguage,
            (),
          ),
        (),
      ),
    )
  | _ => None
  };

let toBody = bodyInput =>
  switch (
    bodyInput##textualBody,
    bodyInput##externalBody,
    bodyInput##choiceBody,
    bodyInput##specificBody,
  ) {
  | (Some(textualBody), None, None, None) =>
    Some(
      `TextualBody({
        "__typename": "TextualBody",
        "id": textualBody##id,
        "value": textualBody##value,
        "purpose": textualBody##purpose,
        "accessibility": textualBody##accessibility,
        "rights": textualBody##rights,
        "format": textualBody##format,
        "textDirection": textualBody##textDirection,
        "language": textualBody##language,
        "processingLanguage": textualBody##processingLanguage,
      }),
    )
  | _ => None
  };

let toCache = bodyInput =>
  switch (
    bodyInput##textualBody,
    bodyInput##externalBody,
    bodyInput##choiceBody,
    bodyInput##specificBody,
  ) {
  | (Some(textualBody), None, None, None) =>
    Js.Dict.fromList([
      ("__typename", "TextualBody"->Js.Json.string),
      ("id", textualBody##id->Js.Json.string),
      ("value", textualBody##value->Js.Json.string),
      (
        "purpose",
        textualBody##purpose
        ->Belt.Option.map(a =>
            a
            ->Belt.Array.map(m =>
                m->Lib_GraphQL_Motivation.toString->Js.Json.string
              )
            ->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "format",
        textualBody##format
        ->Belt.Option.map(f => f->LiteralModel.Format.toString->Js.Json.string)
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "accessibility",
        textualBody##accessibility
        ->Belt.Option.map(a =>
            a->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "rights",
        textualBody##rights
        ->Belt.Option.map(r =>
            r->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "textDirection",
        textualBody##textDirection
        ->Belt.Option.map(t =>
            t->Lib_GraphQL_TextDirection.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "language",
        textualBody##language
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "processingLanguage",
        textualBody##processingLanguage
        ->Belt.Option.map(l =>
            l->Lib_GraphQL_Language.toString->Js.Json.string
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
    ])
    ->Js.Json.object_
    ->Js.Option.some
  | _ => None
  };
