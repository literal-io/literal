module Annotation = {
  /** io.literal.model.Annotation **/
  module Target = {
    [@decco]
    type textualTarget = {
      id: option(string),
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

    type t =
      | TextualTarget(textualTarget)
      | NotImplemented_Passthrough(Js.Json.t);

    let decoder = json =>
      switch (json->Js.Json.classify) {
      | JSONObject(_) =>
        switch (textualTarget_decode(json)) {
        | Ok(textualTarget) => Ok(TextualTarget(textualTarget))
        | _ => Ok(NotImplemented_Passthrough(json))
        }
      | _ =>
        Error({
          Decco.path: "",
          message: "Expected JSONObject for target",
          value: json,
        })
      };

    let encoder = inst =>
      switch (inst) {
      | TextualTarget(target) => textualTarget_encode(target)
      | NotImplemented_Passthrough(json) => json
      };

    let codec: Decco.codec(t) = (encoder, decoder);
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

  let decode = t_decode;
  let encode = t_encode;
};
