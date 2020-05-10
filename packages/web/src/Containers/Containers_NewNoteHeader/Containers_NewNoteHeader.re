open Styles;

external castToListHighlights:
  Js.Json.t => QueryRenderers_Notes_GraphQL.ListHighlightsQuery.t =
  "%identity";

module ListHighlightsCacheReadQuery =
  ApolloClient.ReadQuery(QueryRenderers_Notes_GraphQL.ListHighlightsQuery);
module ListHighlightsCacheWriteQuery =
  ApolloClient.WriteQuery(QueryRenderers_Notes_GraphQL.ListHighlightsQuery);

[@react.component]
let make = (~highlightFragment as highlight) => {
  let (deleteHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_NoteHeader_GraphQL.DeleteHighlightMutation.definition,
    );

  let handleClose = () => {
    let variables =
      Containers_NoteHeader_GraphQL.DeleteHighlightMutation.makeVariables(
        ~input={"id": highlight##id},
        (),
      );

    let _ =
      deleteHighlightMutation(~variables, ())
      |> Js.Promise.then_(_ => {
           let query = QueryRenderers_Notes_GraphQL.ListHighlightsQuery.make();
           let readQueryOptions = ApolloHooks.toReadQueryOptions(query);
           let _ =
             switch (
               ListHighlightsCacheReadQuery.readQuery(
                 Provider.client,
                 readQueryOptions,
               )
             ) {
             | exception _ => ()
             | cachedResponse =>
               switch (cachedResponse->Js.Nullable.toOption) {
               | None => ()
               | Some(cachedListHighlights) =>
                 let listHighlights =
                   castToListHighlights(cachedListHighlights);
                 let updatedListHighlights =
                   listHighlights##listHighlights
                   ->Belt.Option.flatMap(i => i##items)
                   ->Belt.Option.map(i => {
                       let update = {
                         "listHighlights":
                           Some({
                             "items":
                               i
                               ->Belt.Array.keep(
                                   fun
                                   | Some(h) => h##id !== highlight##id
                                   | None => false,
                                 )
                               ->Js.Option.some,
                           }),
                       };
                       Ramda.mergeDeepLeft(update, listHighlights);
                     });
                 let _ =
                   switch (updatedListHighlights) {
                   | Some(updatedListHighlights) =>
                     ListHighlightsCacheWriteQuery.make(
                       ~client=Provider.client,
                       ~variables=query##variables,
                       ~data=updatedListHighlights,
                       (),
                     )
                   | None => ()
                   };
                 ();
               }
             };
           Js.Promise.resolve();
         })
      |> Js.Promise.then_(_ => {
           let _ =
             Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH")));
           Js.Promise.resolve();
         });
    ();
  };

  <Header className={cn(["py-2", "mx-6"])}>
    <MaterialUi.IconButton
      size=`Small
      edge=`Start
      onClick={_ => handleClose()}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes=[Root(cn(["p-0"]))]>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["w-8", "h-8", "pointer-events-none"])}
        icon=Svg.close
      />
    </MaterialUi.IconButton>
  </Header>;
};
