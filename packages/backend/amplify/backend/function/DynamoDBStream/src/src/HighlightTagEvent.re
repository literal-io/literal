module GetTagQuery = [%graphql
  {|
    query GetTag($id: ID!) {
      getTag(id: $id) {
        highlights(limit: 1) {
          items {
            id
          }
        }
      }
    }
  |}
];

module DeleteTagMutation = [%graphql
  {|
    mutation DeleteTag($id: ID!) {
      deleteTag(input: { id: $id }) {
        id
      }
    }
  |}
];

let onRemove = (db: Lambda.highlightTagAttributeMap) => {
  let query = GetTagQuery.make(~id=db.tagId.string, ());
  GraphQL.request(
    ~query=query##query,
    ~variables=query##variables,
    ~operationName="GetTag",
  )
  |> Js.Promise.then_(r => {
        Js.log2("request result", Js.Json.stringify(r));
       let shouldRetainHighlight =
         r
         ->Js.Json.decodeObject
         ->Belt.Option.flatMap(o => o->Js.Dict.get("data"))
         ->Belt.Option.flatMap(d => d->GetTagQuery.parse->(r => r##getTag))
         ->Belt.Option.flatMap(t => t##highlights)
         ->Belt.Option.flatMap(h => h##items)
         ->Belt.Option.map(i => {
             let len =
               i->Belt.Array.keep(Belt.Option.isSome)->Js.Array2.length;
             len > 0;
           })
         ->Belt.Option.getWithDefault(false);

       if (!shouldRetainHighlight) {
         let mutation = DeleteTagMutation.make(~id=db.tagId.string, ());
         GraphQL.request(
           ~query=mutation##query,
           ~variables=mutation##variables,
           ~operationName="DeleteTag",
         )
         |> Js.Promise.then_(r => {
              Js.log2("deletion result", Js.Json.stringify(r));
              r->Js.Option.some->Js.Promise.resolve;
            });
       } else {
         Js.Promise.resolve(None);
       };
     })
  |> Js.Promise.then_(r => {
       let _ =
         switch (r) {
         | Some(_) => Js.log2("Deleted Tag: ", db.tagId.string)
         | None => Js.log2("Retained Tag: ", db.tagId.string)
         };
       Js.Promise.resolve();
     });
};