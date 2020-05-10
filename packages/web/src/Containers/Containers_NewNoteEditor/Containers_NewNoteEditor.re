open Styles;
open Containers_NewNoteEditor_GraphQL;

external castToListHighlights:
  Js.Json.t => QueryRenderers_Notes_GraphQL.ListHighlightsQuery.t =
  "%identity";

module ListHighlightsCacheReadQuery =
  ApolloClient.ReadQuery(QueryRenderers_Notes_GraphQL.ListHighlightsQuery);
module ListHighlightsCacheWriteQuery =
  ApolloClient.WriteQuery(QueryRenderers_Notes_GraphQL.ListHighlightsQuery);

[@react.component]
let make = (~highlightFragment as highlight) => {
  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let (textState, setTextState) = React.useState(() => {highlight##text});

  let handleSave = () => {
    let variables =
      UpdateHighlightMutation.makeVariables(
        ~input={
          "id": highlight##id,
          "text": textState->Js.Option.some,
          "createdAt": None,
          "note": None,
          "highlightScreenshotId": None,
        },
        (),
      );
    let _ =
      updateHighlightMutation(~variables, ())
      |> Js.Promise.then_(_ => {
           let _ =
             Webview.(postMessage(WebEvent.make(~type_="ACTIVITY_FINISH")));
           Js.Promise.resolve();
         });
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
