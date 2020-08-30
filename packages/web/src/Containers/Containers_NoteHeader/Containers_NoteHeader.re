open Containers_NoteHeader_GraphQL;
open Styles;

/**
external castToListHighlights:
  Js.Json.t => QueryRenderers_Notes_GraphQL.ListHighlights.Query.t =
  "%identity";
**/

[@react.component]
let make = (~annotationFragment as annotation=?, ~currentUser=?) => {
  let (deleteAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(
      Containers_NoteHeader_GraphQL.DeleteAnnotationMutation.definition,
    );

  let handleCreate = () => {
    let _ = Next.Router.push("/notes/new");
    ();
  };

  let handleDelete = (~annotation, ~currentUser) => {
    let variables =
      DeleteAnnotationMutation.makeVariables(
        ~input={
          "creatorUsername": currentUser.username,
          "id": annotation##id
        },
        (),
      );

    let _ =
      deleteHighlightMutation(~variables, ())
      |> Js.Promise.then_(_ => {
            /** FIXME: restore cache update
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
             **/
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
          "font-semibold",
          "italic",
          "leading-none",
          "text-xl",
        ])}>
        {React.string("#recent")}
      </h1>
      <div className={cn(["flex", "flex-row"])}>
        <MaterialUi.IconButton
          size=`Small
          edge=MaterialUi.IconButton.Edge._end
          onClick={_ =>
            switch (annotation, currentUser) {
            | (Some(annotation), Some(currentUser)) =>
              handleDelete(~annotation, ~currentUser)
            | _ => ()
            }
          }
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(
            ~root=cn(["p-0", "ml-1"]),
            (),
          )}>
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
        <MaterialUi.IconButton
          size=`Small
          edge=MaterialUi.IconButton.Edge._end
          onClick={_ =>
            switch (highlight, currentUser) {
            | (Some(_), Some(_)) => handleCreate()
            | _ => ()
            }
          }
          _TouchRippleProps={
            "classes": {
              "child": cn(["bg-white"]),
              "rippleVisible": cn(["opacity-50"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(
            ~root=cn(["p-0", "ml-4"]),
            (),
          )}>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={cn(["pointer-events-none", "opacity-75"])}
            style={ReactDOMRe.Style.make(
              ~width="1.75rem",
              ~height="1.75rem",
              (),
            )}
            icon=Svg.add
          />
        </MaterialUi.IconButton>
      </div>
    </div>
  </Header>;
};
