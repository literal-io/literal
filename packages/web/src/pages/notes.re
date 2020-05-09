open Styles;

module Data = {
  [@react.component]
  let make = (~highlights) => {
    highlights
    ->Belt.Array.map(h =>
        <Next.Link key={h##id} href="notes/[id]" _as={"notes/" ++ h##id}>
          <a
            className={cn([
              "py-4",
              "px-6",
              "border-b",
              "border-white",
              "block",
            ])}>
            <HighlightText> {React.string(h##text)} </HighlightText>
          </a>
        </Next.Link>
      )
    ->React.array;
  };
};

module Empty = {
  [@react.component]
  let make = () => React.string("Not Found...");
};

module Loading = {
  [@react.component]
  let make = () => React.string("Loading...");
};

/**
 * FIXME: We're ordering on createdAt on the client, so this will break
 * once results are paginated. Requires tweaking indices in Dynamo to
 * sort in DB.
 */
module ListHighlightsQuery = [%graphql
  {|
    query ListHighlights {
      listHighlights(limit: 100) {
        items {
          id
          text
          createdAt
        }
      }
    }
  |}
];

[@react.component]
let default = () => {
  let (query, _fullQuery) =
    ApolloHooks.useQuery(ListHighlightsQuery.definition);

  <div className={cn(["w-full", "h-full", "bg-black", "overflow-y-scroll"])}>
    {switch (query) {
     | Data(data) =>
       switch (data##listHighlights->Belt.Option.flatMap(h => h##items)) {
       | Some(highlights) =>
         <Data
           highlights={
             highlights->Belt.Array.keepMap(i => i)
             |> Ramda.sortBy(h => {
                  -. h##createdAt->Js.Date.fromString->Js.Date.valueOf
                })
           }
         />
       | None => <Empty />
       }
     | Loading => <Loading />
     | NoData
     | Error(_) => <Empty />
     }}
  </div>;
};
