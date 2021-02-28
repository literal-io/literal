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
