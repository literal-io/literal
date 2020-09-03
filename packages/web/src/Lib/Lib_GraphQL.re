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

  let targetInputFromTarget = target =>
    switch (target) {
    | `TextualTarget(target) => {
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
    | `ExternalTarget(target) => {
        "textualTarget": None,
        "externalTarget":
          Some({
            "id": target##id,
            "format": target##format,
            "language": target##language,
            "processingLanguage": target##processingLanguage,
            "textDirection": target##textDirection,
            "accessibility": target##accessibility,
            "rights": target##rights,
            "type": target##type_,
          }),
      }
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
