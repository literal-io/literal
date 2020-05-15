open Styles;

external castToListHighlights:
  Js.Json.t => QueryRenderers_Notes_GraphQL.ListHighlights.Query.t =
  "%identity";

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
           let _ =
             switch (
               QueryRenderers_Notes_GraphQL.ListHighlights.readCache(
                 Provider.client,
               )
             ) {
             | None => ()
             | Some(cachedListHighlights) =>
               let updatedListHighlights =
                 QueryRenderers_Notes_GraphQL.ListHighlights.Raw.(
                   cachedListHighlights
                   ->listHighlights
                   ->Belt.Option.flatMap(items)
                   ->Belt.Option.map(items => {
                       let updatedItems =
                         items
                         ->Belt.Array.keep(
                             fun
                             | Some(h) => h.id !== highlight##id
                             | None => false,
                           )
                         ->Js.Option.some;
                       {
                         listHighlights:
                           Some({
                             ...
                               cachedListHighlights
                               ->listHighlights
                               ->Belt.Option.getExn,
                             items: updatedItems,
                           }),
                       };
                     })
                 );
               let _ =
                 switch (updatedListHighlights) {
                 | Some(updatedListHighlights) =>
                   QueryRenderers_Notes_GraphQL.ListHighlights.(
                     writeCache(
                       ~client=Provider.client,
                       ~data=updatedListHighlights,
                     )
                   )
                 | None => ()
                 };
               ();
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
