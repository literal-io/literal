let origin = Webapi.Dom.(window |> Window.location |> Location.origin);
let makeHash = text =>
  Externals_Crypto.(digest("SHA-256", encode(makeTextEncoder(), text)));

module Annotation = {
  let defaultContext = "http://www.w3.org/ns/anno.jsonld";

  let makeId = (~creatorUsername, ~textualTargetValue) =>
    makeHash(textualTargetValue)
    |> Js.Promise.then_(valueHash => {
         Js.Promise.resolve(
           origin
           ++ "/creators/"
           ++ creatorUsername
           ++ "/annotations/"
           ++ valueHash,
         )
       });

  let isBodyTag = body => {
    let hasTaggingPurpose =
      body##purpose
      ->Belt.Option.map(purpose =>
          purpose->Belt.Array.some(p => p === `TAGGING)
        )
      ->Belt.Option.getWithDefault(false);

    body##__typename === "TextualBody"
    && Js.Option.isSome(body##id)
    && hasTaggingPurpose;
  };

  /**
   * Weird polymorphic variant required due to how graphql_ppx handles
   * union type polymorphism. Input field types are polymorphic (e.g.
   * `field`), but ocaml derives from first usage.
   */
  type targetInput('a, 'b) =
    | TextualTarget(Js.t('a))
    | ExternalTarget(Js.t('b));

  let targetInputFromTarget = target => {
    let conv =
      switch (target##__typename) {
      | "TextualTarget" => Some(TextualTarget(target))
      | "ExternalTarget" => Some(ExternalTarget(target))
      | _ => None
      };

    switch (conv) {
    | Some(TextualTarget(target)) => {
        "textualTarget":
          Some({
            "id": target##id,
            "format": target##format,
            "processingLanguage": target##processingLanguage,
            "language": target##language,
            "textDirection": target##textDirection,
            "accessibility": target##accessibility,
            "rights": target##rights,
            "value": target##value,
          }),
        "externalTarget": None,
      }
    | Some(ExternalTarget(target)) => {
        "textualTarget": None,
        "externalTarget":
          Some({
            "id": target##id->Js.Option.getExn,
            "format": target##format,
            "language": target##language,
            "processingLanguage": target##processingLanguage,
            "textDirection": target##textDirection,
            "accessibility": target##accessibility,
            "rights": target##rights,
            "type": target##type_,
          }),
      }
    | None => {"textualTarget": None, "externalTarget": None}
    };
  };
};

module AnnotationCollection = {
  let makeId = (~creatorUsername, ~label) =>
    makeHash(label)
    |> Js.Promise.then_(hash =>
         Js.Promise.resolve(
           origin
           ++ "/creators/"
           ++ creatorUsername
           ++ "/annotation-collections/"
           ++ hash,
         )
       );
};
