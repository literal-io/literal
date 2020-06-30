let makeTagId = (~text, ~owner) => owner ++ "-" ++ text;

let makeHighlightTagId = (~highlightId, ~tagId) =>
  highlightId ++ "-" ++ tagId;

module GetTagFragment = [%graphql
  {|
    fragment GetTag on Tag {
      id
    }
  |}
];


let shouldCreateTag = tag => {
  switch (
    Apollo.Client.(
      readFragment(
        Provider.client,
        {
          id: "Tag:" ++ tag##id,
          fragment: ApolloClient.gql(. GetTagFragment.GetTag.query),
        },
      )
    )
  ) {
  | Some(_) => false
  | None => true
  | exception _ => true
  };
};

