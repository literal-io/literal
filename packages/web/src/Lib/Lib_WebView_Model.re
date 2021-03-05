module Annotation = {
  module Selector = {
    [@decco]
    type rangeSelector = {
      startSelector: t,
      endSelector: t,
      [@decco.key "type"]
      type_: string,
    }
    [@decco]
    and xPathSelector = {
      value: string,
      refinedBy: option(array(t)),
      [@decco.key "type"]
      type_: string,
    }
    [@decco]
    and textPositionSelector = {
      [@decco.key "end"]
      end_: int,
      start: int,
      [@decco.key "type"]
      type_: string,
    }
    [@decco]
    and t =
      | RangeSelector(rangeSelector)
      | XPathSelector(xPathSelector)
      | TextPositionSelector(textPositionSelector)
      | NotImplemented_Passthrough(Js.Json.t);

    let t_decode = json =>
      switch (json->Js.Json.classify) {
      | JSONObject(_) =>
        switch (rangeSelector_decode(json)) {
        | Ok(rangeSelector) => Ok(RangeSelector(rangeSelector))
        | _ =>
          switch (xPathSelector_decode(json)) {
          | Ok(xPathSelector) => Ok(XPathSelector(xPathSelector))
          | _ =>
            switch (textPositionSelector_decode(json)) {
            | Ok(textPositionSelector) =>
              Ok(TextPositionSelector(textPositionSelector))
            | _ => Ok(NotImplemented_Passthrough(json))
            }
          }
        }
      | _ =>
        Error({
          Decco.path: "",
          message: "Expected JSONObject for selector.",
          value: json,
        })
      };

    let t_encode = inst =>
      switch (inst) {
      | RangeSelector(target) => rangeSelector_encode(target)
      | XPathSelector(target) => xPathSelector_encode(target)
      | TextPositionSelector(target) => textPositionSelector_encode(target)
      | NotImplemented_Passthrough(json) => json
      };

    %raw
    /**
     * FIXME: This is a hack to override the codecs used by t, which seems to require the @decco annotation,
     * even though we want to use the custom variant encoder defined above (t_encode, t_decode)
     */
    {| t_encode = t_encode$1; t_decode = t_decode$1 |};

    let codec: Decco.codec(t) = (t_encode, t_decode);
  };

  /** io.literal.model.Annotation **/
  module Target = {
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
      [@decco.key "type"]
      type_: string,
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
    };

    [@decco]
    type specificTarget = {
      id: string,
      source: t,
      selector: array([@decco.codec Selector.codec] Selector.t),
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
    {| t_encode$2 = t_encode$3; t_decode$2 = t_decode$3 |};

    let codec: Decco.codec(t) = (t_encode, t_decode);
    let encode = t_encode;
    let decode = t_decode;
  };

  module Body = {
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
  };

  [@decco]
  type t = {
    id: option(string),
    target: array([@decco.codec Target.codec] Target.t),
    body: option(array([@decco.codec Body.codec] Body.t)),
  };

  let encode = t_encode;
  let decode = t_decode;
};
