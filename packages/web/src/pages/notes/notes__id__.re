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

module Data = {
  [@react.component]
  let make = (~highlight) => {
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
      <div className={cn(["border-white", "border-b", "py-2"])}>
        <TextEditor
          contentState={Draft.ContentState.createFromText(highlight##text)}
          editorKey="highlight"
          decorator={Draft.makeCompositeDecorator([|
            HighlightTextDecorator.decoratorInput,
          |])}
        />
      </div>
    </div>;
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
    switch (data##getHighlight) {
    | Some(highlight) => <Data highlight />
    | None => <Empty />
    }
  | Loading => <Loading />
  | NoData
  | Error(_) => <Empty />
  };
};
