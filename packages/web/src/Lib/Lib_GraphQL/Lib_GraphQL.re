external castToArrayLike:
  Js.TypedArray2.Uint8Array.t => Js.Array2.array_like(int) =
  "%identity";

[@bs.send] external padStart: (string, int, string) => string = "padStart";

let makeHash = (~digest="SHA-256", text) => {
  text
  |> Externals_Crypto.encode(Externals_Crypto.makeTextEncoder())
  |> Externals_Crypto.digest(digest)
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

  let makeId = (~identityId, ~textualTargetValue) =>
    makeHash(textualTargetValue)
    |> Js.Promise.then_(valueHash => {
         Js.Promise.resolve(
           Constants.apiOrigin
           ++ "/creators/"
           ++ identityId
           ++ "/annotations/"
           ++ valueHash,
         )
       });

  let makeIdFromComponent = (~identityId, ~annotationIdComponent) =>
    Constants.apiOrigin
    ++ "/creators/"
    ++ identityId
    ++ "/annotations/"
    ++ annotationIdComponent;

  /** returns just the last path of the id IRI **/
  let idComponent = fullId =>
    fullId->Js.String2.split("/")->(arr => arr[Js.Array2.length(arr) - 1]);

  let isBodyTag = body => {
    let hasTaggingPurpose =
      body##purpose
      ->Belt.Option.map(purpose =>
          purpose->Belt.Array.some(p => p === `TAGGING)
        )
      ->Belt.Option.getWithDefault(false);

    body##__typename === "TextualBody" && hasTaggingPurpose;
  };

  let annotationFromCreateAnnotationInput = [%raw
    {|
      function (input) {
        return {
          ...input,
          __typename: "Annotation",
          created: (new Date()).toISOString(),
          modified: (new Date()).toISOString(),
          body:
            input.body
              ? input.body.map(body => {
                  const parser = [
                    ["textualBody", {
                      __typename: "TextualBody",
                      accessibility: null,
                      rights: null
                    }],
                    ["choiceBody", {__typename: "ChoiceBody" }],
                    ["externalBody", {__typename: "ExternalBody"}],
                    ["specificBody", {__typename: "SpecificBody"}]
                  ]
                  const [key, attrs] = parser.find(([key, _]) => body[key])
                  return { ...attrs, ...body[key] }
              })
              : null,
          target:
            input.target
              ? input.target.map(target => {
                  const parser = [
                    ["textualTarget", {
                      __typename: "TextualTarget",
                      textualTargetId: null,
                      rights: null,
                      accessibility: null
                    }],
                    ["externalTarget", {__typename: "ExternalTarget" }],
                  ]
                  const [key, attrs] = parser.find(([key, _]) => target[key])
                  return { ...attrs, ...target[key] }

                })
              : null
        }
      }
    |}
  ];
};

module AnnotationCollection = {
  let makeId = (~identityId, ~label) =>
    makeHash(label)
    |> Js.Promise.then_(hash =>
         Js.Promise.resolve(
           Constants.apiOrigin
           ++ "/creators/"
           ++ identityId 
           ++ "/annotation-collections/"
           ++ hash,
         )
       );

  let makeIdFromComponent =
      (
        ~identityId,
        ~annotationCollectionIdComponent,
        ~origin=Constants.apiOrigin,
        (),
      ) =>
    origin
    ++ "/creators/"
    ++ identityId 
    ++ "/annotation-collections/"
    ++ annotationCollectionIdComponent;

  /** returns just the last path of the id IRI **/
  let idComponent = fullId =>
    fullId->Js.String2.split("/")->(arr => arr[Js.Array2.length(arr) - 1]);

  let recentAnnotationCollectionIdComponent = "034a7e52c5c9534b709dc1dba403868399b0949f7c1933a67325c22077ffc221";
  let recentAnnotationCollectionLabel = "recent";
};
