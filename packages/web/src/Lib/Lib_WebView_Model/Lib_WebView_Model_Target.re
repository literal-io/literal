/** io.literal.model.Annotation **/
[@decco]
type textualTarget = {
  id: string,
  format: option(string),
  language: option(string),
  processingLanguage: option(string),
  textDirection: option(string),
  accessibility: option(array(string)),
  rights: option(array(string)),
  value: string,
  [@decco.default "TextualTarget"] [@decco.key "__typename"]
  typename: string,
};

[@decco]
type externalTarget = {
  id: string,
  language: option(string),
  processingLanguage: option(string),
  textDirection: option(string),
  format: option(string),
  accessibility: option(array(string)),
  rights: option(array(string)),
  [@decco.key "type"]
  type_: option(string),
  [@decco.default "ExternalTarget"] [@decco.key "__typename"]
  typename: string,
};

[@decco]
type specificTarget = {
  id: string,
  source: t,
  selector:
    array(
      [@decco.codec Lib_WebView_Model_Selector.codec] Lib_WebView_Model_Selector.t,
    ),
  [@decco.default "SpecificTarget"] [@decco.key "__typename"]
  typename: string,
}
[@decco]
and t =
  | TextualTarget(textualTarget)
  | SpecificTarget(specificTarget)
  | ExternalTarget(externalTarget)
  | NotImplemented_Passthrough(Js.Json.t);

let t_decode = json => {
  switch (json->Js.Json.classify) {
  | JSONObject(_) =>
    switch (textualTarget_decode(json)) {
    | Ok(textualTarget) => Ok(TextualTarget(textualTarget))
    | _ =>
      switch (specificTarget_decode(json)) {
      | Ok(specificTarget) => Ok(SpecificTarget(specificTarget))
      | _ =>
        switch (externalTarget_decode(json)) {
        | Ok(externalTarget) => Ok(ExternalTarget(externalTarget))
        | _ => Ok(NotImplemented_Passthrough(json))
        }
      }
    }
  | _ =>
    Error({
      Decco.path: "",
      message: "Expected JSONObject for target",
      value: json,
    })
  };
};

let makeTextualTarget =
    (
      ~id,
      ~value,
      ~format=?,
      ~language=?,
      ~processingLanguage=?,
      ~textDirection=?,
      ~accessibility=?,
      ~rights=?,
      (),
    ) => {
  id,
  value,
  format,
  language,
  processingLanguage,
  textDirection,
  accessibility,
  rights,
  typename: "TextualTarget",
};

let makeExternalTarget =
    (
      ~id,
      ~language=?,
      ~processingLanguage=?,
      ~textDirection=?,
      ~format=?,
      ~accessibility=?,
      ~rights=?,
      ~type_=?,
      (),
    ) => {
  id,
  language,
  processingLanguage,
  textDirection,
  format,
  accessibility,
  rights,
  type_,
  typename: "ExternalTarget",
};

let makeSpecificTarget = (~id, ~source, ~selector, ()) => {
  id,
  source,
  selector,
  typename: "SpecificTarget",
};

let t_encode = inst =>
  switch (inst) {
  | TextualTarget(target) => textualTarget_encode(target)
  | SpecificTarget(target) => specificTarget_encode(target)
  | ExternalTarget(target) => externalTarget_encode(target)
  | NotImplemented_Passthrough(json) => json
  };

%raw
/**
     * FIXME: This is a hack to override the codecs used by t, which seems to require the @decco annotation,
     * even though we want to use the custom variant encoder defined above (t_encode, t_decode)
     */
{| t_encode = t_encode$1; t_decode = t_decode$1 |};

let codec: Decco.codec(t) = (t_encode, t_decode);
let encode = t_encode;
let decode = t_decode;
