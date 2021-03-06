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
  hashId: option(string),
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
    option(
      array(
        [@decco.codec Selector.codec] Selector.t,
      ),
    ),
  state: option(array(State.t)),
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

let makeTextualTargetFromGraphQL = textualTarget =>
  TextualTarget(
    makeTextualTarget(
      ~id=textualTarget##textualTargetId,
      ~value=textualTarget##value,
      ~language=?
        textualTarget##language
        ->Belt.Option.map(Language.toString),
      ~processingLanguage=?
        textualTarget##processingLanguage
        ->Belt.Option.map(Language.toString),
      ~accessibility=?textualTarget##accessibility,
      ~rights=?textualTarget##rights,
      ~textDirection=?
        textualTarget##textDirection
        ->Belt.Option.map(TextDirection.toString),
      ~format=?
        textualTarget##format->Belt.Option.map(Format.toString),
      (),
    ),
  );

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
      ~hashId=?,
      (),
    ) => {
  id,
  hashId,
  language,
  processingLanguage,
  textDirection,
  format,
  accessibility,
  rights,
  type_,
  typename: "ExternalTarget",
};

let makeExternalTargetFromGraphQL = externalTarget => {
  ExternalTarget(
    makeExternalTarget(
      ~id=externalTarget##externalTargetId,
      ~language=?
        externalTarget##language
        ->Belt.Option.map(Language.toString),
      ~processingLanguage=?
        externalTarget##processingLanguage
        ->Belt.Option.map(Language.toString),
      ~accessibility=?externalTarget##accessibility,
      ~rights=?externalTarget##rights,
      ~textDirection=?
        externalTarget##textDirection
        ->Belt.Option.map(TextDirection.toString),
      ~format=?
        externalTarget##format->Belt.Option.map(Format.toString),
      ~type_=?
        externalTarget##type_->Belt.Option.map(ResourceType.toJs),
      (),
    ),
  );
};

let makeSpecificTarget = (~id, ~source, ~selector, ~state, ()) => {
  id,
  source,
  selector,
  state,
  typename: "SpecificTarget",
};

let t_encode = inst =>
  switch (inst) {
  | TextualTarget(target) => textualTarget_encode(target)
  | SpecificTarget(target) => specificTarget_encode(target)
  | ExternalTarget(target) => externalTarget_encode(target)
  | NotImplemented_Passthrough(json) => json
  };

let makeSpecificTargetFromGraphQL =
    (~makeSelector, ~makeState, specificTarget) => {
  let selector =
    specificTarget##selector
    ->Belt.Array.keepMap(makeSelector)
    ->Js.Option.some;
  let state =
    specificTarget##state
    ->Belt.Option.map(a => a->Belt.Array.keepMap(makeState));

  let externalTarget =
    switch (specificTarget##source) {
    | `ExternalTarget(target) => Some(makeExternalTargetFromGraphQL(target))
    | `Nonexhaustive => None
    };

  switch (externalTarget) {
  | Some(externalTarget) =>
    Some(
      SpecificTarget(
        makeSpecificTarget(
          ~id=specificTarget##specificTargetId,
          ~source=externalTarget,
          ~selector,
          ~state,
          (),
        ),
      ),
    )
  | _ => None
  };
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
