open Styles;
open Containers_NewNoteEditor_GraphQL;

[@react.component]
let make = (~currentUser) => {
  let (createHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(CreateHighlightMutation.definition);

  let (textState, setTextState) = React.useState(() => "");

  let handleSave = () => {
    let highlightId = Uuid.makeV4();
    let variables =
      CreateHighlightMutation.makeVariables(
        ~input={
          "id": highlightId,
          "text": textState,
          "createdAt": None,
          "note": None,
          "highlightScreenshotId": None,
          "owner": None,
        },
        (),
      );
    let _ = createHighlightMutation(~variables, ());
    let cacheQuery =
      QueryRenderers_Notes_GraphQL.ListHighlights.Query.make(
        ~owner=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        (),
      );
    let _ =
      switch (
        QueryRenderers_Notes_GraphQL.ListHighlights.readCache(
          ~query=cacheQuery,
          ~client=Provider.client,
          (),
        )
      ) {
      | None => ()
      | Some(cachedQuery) =>
        let updatedListHighlights =
          QueryRenderers_Notes_GraphQL.ListHighlights.Raw.(
            cachedQuery
            ->listHighlights
            ->Belt.Option.flatMap(items)
            ->Belt.Option.map(items => {
                let updatedItems =
                  Js.Array.concat(
                    [|
                      Some({
                        id: highlightId,
                        createdAt: Js.Date.(make()->toISOString),
                        text: textState,
                        typename: highlightTypename,
                      }),
                    |],
                    items,
                  );
                {
                  ...cachedQuery,
                  listHighlights:
                    Some({
                      ...cachedQuery->listHighlights->Belt.Option.getExn,
                      items: Some(updatedItems),
                    }),
                };
              })
          );
        let _ =
          switch (updatedListHighlights) {
          | Some(updatedListHighlights) =>
            QueryRenderers_Notes_GraphQL.ListHighlights.writeCache(
              ~client=Provider.client,
              ~data=updatedListHighlights,
              ~query=cacheQuery,
              (),
            )
          | None => ()
          };
        ();
      };

    let _ = Next.Router.back();
    ();
  };

  let handleTextChange = s => setTextState(_ => s);

  <>
    <div className={cn(["px-6", "pt-4", "pb-24"])}>
      <TextInput.Basic onChange=handleTextChange value=textState />
    </div>
    <FloatingActionButton
      onClick={_ev => handleSave()}
      className={cn(["fixed", "right-0", "bottom-0", "m-6", "z-10"])}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["w-10", "h-10", "pointer-events-none"])}
        icon=Svg.done_
      />
    </FloatingActionButton>
  </>;
};
