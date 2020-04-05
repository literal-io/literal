module Data = {
  [@react.component]
  let make = (~highlights) =>
    highlights
    ->Belt.Array.map(h => <span> {React.string(h##text)} </span>)
    ->React.array;
};

module Empty = {
  [@react.component]
  let make = () => React.string("Not Found...");
};

module Loading = {
  [@react.component]
  let make = () => React.string("Loading...");
};

module ListHighlightsQuery = [%graphql
  {|
    query ListHighlights {
      listHighlights {
        items {
          id
          text
        }
      }
    }
  |}
];

[@react.component]
let default = () => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(ListHighlightsQuery.definition);

  switch (query) {
  | Data(data) =>
    switch (data##listHighlights->Belt.Option.flatMap(h => h##items)) {
    | Some(highlights) =>
      <Data highlights={highlights->Belt.Array.keepMap(i => i)} />
    | None => <Empty />
    }
  | Loading => <Loading />
  | NoData
  | Error(_) => <Empty />
  };
};
