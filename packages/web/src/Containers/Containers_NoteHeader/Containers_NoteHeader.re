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

  let handleDelete = () => {
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
             | exception e => Error.report(e)
             | cachedResponse =>
               switch (cachedResponse->Js.Nullable.toOption) {
               /*** expected if direct link to this highlight **/
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
         });
    /**let _ = Next.Router.back(); **/ ();
  };

  <Header
    className={cn([
      "justify-between",
      "absolute",
      "left-0",
      "right-0",
      "top-0",
    ])}>
    {React.string("#recent")}
    <MaterialUi.IconButton
      size=`Small
      edge=`End
      onClick={_ => handleDelete()}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-50"]),
        },
      }
      classes=[Root(cn(["p-0", "ml-1"]))]>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "opacity-75"])}
        style={ReactDOMRe.Style.make(~width="1.75rem", ~height="1.75rem", ())}
        icon=Svg.delete
      />
    </MaterialUi.IconButton>
  </Header>;
};
