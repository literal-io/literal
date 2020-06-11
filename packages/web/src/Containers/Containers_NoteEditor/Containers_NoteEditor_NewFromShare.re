open Styles;
open Containers_NoteEditor_NewFromShare_GraphQL;
open Containers_NoteEditor_GraphQL_Util;

[@react.component]
let make = (~highlightFragment as highlight, ~currentUser) => {
  let (editorValue, setEditorValue) =
    React.useState(() => Containers_NoteEditor_Base.{text: "", tags: [||]});

  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let handleSave = () => {
    let updateHighlightInput = {
      "id": highlight##id,
      "text": editorValue.text->Js.Option.some,
      "createdAt": None,
      "note": None,
      "highlightScreenshotId": None,
      "owner": None,
    };
    let createTagsInput =
      editorValue.tags
      ->Belt.Array.map(tag =>
          {"id": tag##id, "text": tag##text, "createdAt": None}
        );
    let createHighlightTagsInput =
      createTagsInput->Belt.Array.map(tag =>
        {
          "id":
            makeHighlightTagId(
              ~highlightId=updateHighlightInput##id,
              ~tagId=tag##id,
            )
            ->Js.Option.some,
          "highlightId": updateHighlightInput##id,
          "tagId": tag##id,
          "createdAt": None,
        }
      );

    let variables =
      UpdateHighlightMutation.makeVariables(
        ~updateHighlightInput,
        ~createTagsInput,
        ~createHighlightTagsInput,
        ~deleteHighlightTagsInput=[||],
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

  let handleChange = value => setEditorValue(_ => value);

  <>
    <div className={cn(["px-6", "pt-4", "pb-24"])}>
      <Containers_NoteEditor_Base
        highlightFragment=highlight
        onChange=handleChange
        autoFocus=true
        placeholder="Lorem Ipsum"
        currentUser
      />
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
