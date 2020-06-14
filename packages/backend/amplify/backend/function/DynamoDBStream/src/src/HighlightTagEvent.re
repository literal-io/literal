module GetTag = [%graphql
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

let onRemove = (db: Lambda.highlightTagAttributeMap) => Js.Promise.resolve();
