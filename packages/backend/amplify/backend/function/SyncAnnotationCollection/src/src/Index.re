/**
let getAnnotationTags = (annotation: External_Lambda.annotation) =>
  annotation.body
  ->Belt.Option.map(bodies =>
      bodies->Belt.Array.keepMap(body => {
        let isTag =
          body.type_ == "TEXTUAL_BODY"
          && body.purpose
             ->Belt.Option.map(p =>
                 p->Belt.Array.some(purpose => purpose === "TAGGING")
               )
             ->Belt.Option.getWithDefault(false);
        isTag ? Some(body) : None;
      })
    )
  ->Belt.Option.getWithDefault([||]);

module CreateAnnotationPage = [%graphql
  {|
    mutation CreateAnnotationPage($input: CreateAnnotationPageInput!) {
      createAnnotationPage(input: $input) {
        id
      }
    }
  |}
];

module CreateAnnotationPageItem = [%graphql
  {|
    mutation CreateAnnotationPageItem($input: CreateAnnotationPageItemInput!) {
      createAnnotationPageItem(input: $input) {
        id
      }
    }
  |}
];

module CreateAnnotationCollection = [%graphql
  {|
    mutation CreateAnnotationCollection($input: CreateAnnotationCollectionInput!) {
      createAnnotationCollection(input: $input) {
        id
      }
    }
  |}
];

**/

let handleCreate = annotation => {
  /**
  annotation
  ->getAnnotationTags
  ->Belt.Array.map(tag => {
      let tagHashId = "";
      ();
    });
  **/
  Js.Promise.resolve(None);
};

let handleDelete = annotation => Js.Promise.resolve(None);

let handleUpdate = (~before, ~after) => Js.Promise.resolve(None);

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));

  switch (event->External_Lambda.event_decode) {
  | Belt.Result.Ok({before: None, after: Some(annotation)}) =>
    handleCreate(annotation)
  | Belt.Result.Ok({before: Some(annotation), after: None}) =>
    handleDelete(annotation)
  | Belt.Result.Ok({before: Some(before), after: Some(after)}) =>
    handleUpdate(~before, ~after)
  | Belt.Result.Ok({before: None, after: None}) =>
    Js.Exn.raiseError("Expected before or after, but found none.")
  | Belt.Result.Error(e) => Js.Exn.raiseError(e.message)
  };
};
