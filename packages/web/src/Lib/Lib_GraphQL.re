external castToArrayLike:
  Js.TypedArray2.Uint8Array.t => Js.Array2.array_like(int) =
  "%identity";

[@bs.send] external padStart: (string, int, string) => string = "padStart";

let makeHash = text => {
  Externals_Crypto.(digest("SHA-256", encode(makeTextEncoder(), text)))
  |> Js.Promise.then_(buffer =>
       buffer
       ->Js.TypedArray2.Uint8Array.fromBuffer
       ->castToArrayLike
       ->Js.Array2.from
       ->Js.Array2.map(b =>
           b->Js.Int.toStringWithRadix(~radix=16)->padStart(2, "0")
         )
       ->Js.Array2.joinWith("")
       ->Js.Promise.resolve
     );
};

module Annotation = {
  let defaultContext = "http://www.w3.org/ns/anno.jsonld";

  let makeId = (~creatorUsername, ~textualTargetValue) =>
    makeHash(textualTargetValue)
    |> Js.Promise.then_(valueHash => {
         Js.Promise.resolve(
           Webapi.Dom.(window |> Window.location |> Location.origin)
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
            "id": target##textualTargetId,
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
            "id": target##externalTargetId,
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
           Webapi.Dom.(window |> Window.location |> Location.origin)
           ++ "/creators/"
           ++ creatorUsername
           ++ "/annotation-collections/"
           ++ hash,
         )
       );
};
