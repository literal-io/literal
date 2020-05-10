open Styles;
open Containers_NoteEditor_GraphQL;

let handleSave =
  Lodash.debounce2(
    (.
      variables,
      updateHighlightMutation:
        ApolloHooks.Mutation.mutation(UpdateHighlightMutation.t),
    ) => {
      let _ = updateHighlightMutation(~variables, ());
      ();
    },
    500,
  );

[@react.component]
let make = (~highlightFragment as highlight) => {
  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let (textState, setTextState) = React.useState(() => {highlight##text});
  let _ =
    React.useEffect1(
      () => {
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

        let _ = handleSave(. variables, updateHighlightMutation);

        None;
      },
      [|textState|],
    );

  let _ =
    React.useEffect0(() => {
      Some(
        () => {
          let _ = Lodash.flush2(handleSave);
          ();
        },
      )
    });

  let handleTextChange = s => setTextState(_ => s);

  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={cn(["px-6", "pb-4", "pt-16"])}>
      <TextInput.Basic onChange=handleTextChange value=textState />
    </div>
  </div>;
};
