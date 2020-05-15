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

  let handleDelete = () => {
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
                   QueryRenderers_Notes_GraphQL.ListHighlights.writeCache(
                     ~client=Provider.client,
                     ~data=updatedListHighlights,
                   )
                 | None => ()
                 };
               ();
             };
           Js.Promise.resolve();
         });
    ();
  };

  <Header
    className={cn([
      "absolute",
      "left-0",
      "right-0",
      "top-0",
      "bg-black",
      "z-10",
    ])}>
    <div
      style={style(~borderColor="rgba(255, 255, 255, 0.5)", ())}
      className={cn([
        "justify-between",
        "items-center",
        "border-b",
        "py-2",
        "mx-6",
        "flex",
        "flex-1",
      ])}>
      <h1
        className={cn([
          "text-white",
          "font-sans",
          "font-bold",
          "italic",
          "leading-none",
          "text-xl",
        ])}>
        {React.string("#recent")}
      </h1>
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
          style={ReactDOMRe.Style.make(
            ~width="1.75rem",
            ~height="1.75rem",
            (),
          )}
          icon=Svg.delete
        />
      </MaterialUi.IconButton>
    </div>
  </Header>;
};
