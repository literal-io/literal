[@decco]
type textualBody = {
  id: option(string),
  format: option(string),
  language: option(string),
  processingLanguage: option(string),
  textDirection: option(string),
  purpose: option(array(string)),
  accessibility: option(array(string)),
  rights: option(array(string)),
  value: string,
  [@decco.key "type"]
  type_: string,
  [@decco.default "TextualBody"] [@decco.key "__typename"]
  typename: string,
};

let makeTextualBody =
    (
      ~value,
      ~id=?,
      ~format=?,
      ~language=?,
      ~processingLanguage=?,
      ~textDirection=?,
      ~purpose=?,
      ~accessibility=?,
      ~rights=?,
      (),
    ) => {
  id,
  value,
  format,
  language,
  processingLanguage,
  purpose,
  accessibility,
  rights,
  textDirection,
  type_: "TEXTUAL_BODY",
  typename: "TextualBody",
};

type t =
  | TextualBody(textualBody)
  | NotImplemented_Passthrough(Js.Json.t);

let decoder = json =>
  switch (json->Js.Json.classify) {
  | JSONObject(_) =>
    switch (textualBody_decode(json)) {
    | Ok(textualBody) => Ok(TextualBody(textualBody))
    | _ => Ok(NotImplemented_Passthrough(json))
    }
  | _ =>
    Error({
      Decco.path: "",
      message: "Expected JSONObject for body",
      value: json,
    })
  };

let encoder = inst =>
  switch (inst) {
  | TextualBody(textualBody) => textualBody_encode(textualBody)
  | NotImplemented_Passthrough(json) => json
  };

let codec: Decco.codec(t) = (encoder, decoder);
