type t = {
  id: option(string),
  href: option(string),
  text: string,
};

let ensureId = (~identityId, tag) => {
  let id =
    switch (tag.id) {
    | Some(id) => Js.Promise.resolve(id)
    | None =>
      Lib_GraphQL.AnnotationCollection.makeId(~identityId, ~label=tag.text)
    };
  id |> Js.Promise.then_(id => Js.Promise.resolve({...tag, id: Some(id)}));
};

let asTextualBody = tag =>
  tag.id
  ->Belt.Option.map(id =>
      `TextualBody({
        "__typename": "TextualBody",
        "id": id,
        "format": Some(`TEXT_PLAIN),
        "language": Some(`EN_US),
        "processingLanguage": Some(`EN_US),
        "textDirection": Some(`LTR),
        "accessibility": None,
        "rights": None,
        "purpose": Some([|`TAGGING|]),
        "value": tag.text,
        "type": Some(`TEXTUAL_BODY),
      })
    );
