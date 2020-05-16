open Containers_Onboarding_GraphQL;

let onboardingNotes = [|
  "Welcome to Literal.\n\nLiteral is an annotation management system. Annotations from books that you read will appear here.\n\nScroll right to learn more.",
  "Literal is best used to capture annotations while you're reading. Literal is agnostic about where and how you currently read text, and supports annotations made within a web browser, PDF reader, and more.\n\nTo create a note, highlight the text and use your device's share dialog to share it to the Literal application. If there is no share dialog, screenshot the highlight and share the screenshot to the Literal application.\n\nScroll right to learn more.",
  "Annotations are organized primarily based on tags and bi-directional links between tags in order to retain context and build connections.\n\nIf you have any questions, reach out to hello@literal.io.\n\nOnce you've created some annotations, feel free to delete these introductory example annotations.",
|];

[@react.component]
let make = () => {
  let (createHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(CreateHighlightMutation.definition);
  let _ =
    React.useEffect0(() => {
      /** enforce desired ordering of onboarding highlights via createdAt timestamps */
      let baseTs = Js.Date.make();
      let _ =
        onboardingNotes
        ->Belt.Array.reverse
        ->Belt.Array.mapWithIndex((idx, text) => {
            let _ = baseTs->Js.Date.setMilliseconds(float_of_int(idx));
            let variables =
              CreateHighlightMutation.makeVariables(
                ~input={
                  "id": Uuid.makeV4(),
                  "text": text,
                  "note": None,
                  "highlightScreenshotId": None,
                  "createdAt": baseTs |> Js.Date.toISOString |> Js.Option.some,
                },
                (),
              );
            createHighlightMutation(~variables, ());
          })
        ->Js.Promise.all
        |> Js.Promise.then_(results => {
             let updatedItems =
               results
               ->Belt.Array.keepMap(
                   (
                     r: ApolloHooks.Mutation.result(CreateHighlightMutation.t),
                   ) =>
                   switch (r) {
                   | Data(mutationResult) =>
                     mutationResult##createHighlight
                     ->Belt.Option.map(highlight =>
                         QueryRenderers_Notes_GraphQL.ListHighlights.Raw.{
                           id: highlight##id,
                           createdAt: highlight##createdAt,
                           text: highlight##editorHighlightFragment##text,
                           typename: highlight##__typename,
                         }
                       )
                   | Error(_)
                   | NoData => None
                   }
                 )
               ->Belt.Array.map(Js.Option.some);
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
                         {
                           listHighlights:
                             Some({
                               ...
                                 cachedListHighlights
                                 ->listHighlights
                                 ->Belt.Option.getExn,
                               items:
                                 Some(
                                   Belt.Array.concat(items, updatedItems),
                                 ),
                             }),
                         }
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
           });
      None;
    });

  <Loading />;
};
