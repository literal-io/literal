open Containers_NoteHeader_GraphQL;
open Styles;

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
          "creatorUsername":
            AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
          "id": annotation##id,
        },
        (),
      );

    let _ = deleteAnnotationMutation(~variables, ());
    let cacheQuery =
      QueryRenderers_Notes_GraphQL.ListAnnotations.Query.make(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        (),
      );
    let _ =
      QueryRenderers_Notes_GraphQL.ListAnnotations.readCache(
        ~query=cacheQuery,
        ~client=Provider.client,
        (),
      )
      ->Belt.Option.flatMap(cachedQuery => cachedQuery##listAnnotations)
      ->Belt.Option.flatMap(listAnnotations => listAnnotations##items)
      ->Belt.Option.forEach(annotations => {
          let newAnnotations =
            annotations->Belt.Array.keep(cachedAnnotation =>
              switch (cachedAnnotation) {
              | Some(cachedAnnotation) =>
                cachedAnnotation##id !== annotation##id
              | None => false
              }
            );
          let newData = {
            "listAnnotations":
              Some({
                "__typename": "ModelAnnotationConnection",
                "items": Some(newAnnotations),
              }),
            "__typename": "Query",
          };
          let _ =
            QueryRenderers_Notes_GraphQL.ListAnnotations.writeCache(
              ~query=cacheQuery,
              ~client=Provider.client,
              ~data=newData,
              (),
            );
          ();
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
            switch (annotation, currentUser) {
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
