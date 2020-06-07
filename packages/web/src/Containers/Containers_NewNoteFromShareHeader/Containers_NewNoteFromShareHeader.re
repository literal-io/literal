open Styles;

[@react.component]
let make = (~highlightFragment as highlight, ~currentUser) => {
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
                   ->Belt.Option.flatMap(highlightConnectionItems)
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
                         ...cachedQuery,
                         listHighlights:
                           Some({
                             ...
                               cachedQuery->listHighlights->Belt.Option.getExn,
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
                       ~query=cacheQuery,
                       ~client=Provider.client,
                       ~data=updatedListHighlights,
                       (),
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
