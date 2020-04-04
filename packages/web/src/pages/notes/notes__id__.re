open Styles;

module GetNoteQuery = [%graphql
  {|
    query GetHighlight($id: ID!) {
      getHighlight(id: $id) {
        text
        note
      }
    }
  |}
];

[@decco]
type routeParams = {id: string};

[@react.component]
let default = () => {
  let router = Next.useRouter();

  let postId =
    switch (routeParams_decode(router.Next.query)) {
    | Ok(p) => Some(p.id)
    | _ => None
    };

  let (query, _fullQuery) =
    ApolloHooks.useQuery(
      ~variables=
        GetNoteQuery.makeVariables(
          ~id=postId->Belt.Option.getWithDefault(""),
          (),
        ),
      GetNoteQuery.definition,
    );

  switch (query) {
  | Data(data) =>
    <div
      className={cn([
        "w-full",
        "h-full",
        "bg-black",
        "px-6",
        "flex",
        "flex-col",
      ])}>
      <Header title="Create" />
      <div className={cn(["my-4"])}> <TextEditor /> </div>
    </div>
  | Loading => React.string("Loading...")
  | NoData
  | Error(_) => React.string("Error...")
  };
};
