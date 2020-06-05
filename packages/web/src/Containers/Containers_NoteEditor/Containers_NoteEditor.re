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
  let (tagsState, setTagsState) =
    React.useState(() =>
      highlight##tags
      ->Belt.Option.flatMap(t => t##items)
      ->Belt.Option.map(t =>
          TextInput.Tags.Value.{
            commits:
              t->Belt.Array.keepMap(t => t)->Belt.Array.map(t => t##tag##text),
            partial: "",
          }
        )
      ->Belt.Option.getWithDefault(
          TextInput.Tags.Value.{commits: [||], partial: ""},
        )
    );

  let _ =
    React.useEffect1(
      () => {
        let highlightTags =
          highlight##tags
          ->Belt.Option.flatMap(t => t##items)
          ->Belt.Option.map(t => t->Belt.Array.keepMap(t => t))
          ->Belt.Option.getWithDefault([||]);

        let (createTagsInput, createHighlightTagsInput) = {
          let tagsToCreate =
            tagsState.commits
            ->Belt.Array.keep(tagText => {
                let alreadyExists =
                  Belt.Array.some(highlightTags, highlightTag =>
                    highlightTag##tag##text === tagText
                  );
                !alreadyExists;
              });

          let createTagsInput =
            tagsToCreate->Belt.Array.map(text =>
              {"id": Uuid.makeV4(), "text": text, "createdAt": None}
            );
          let createHighlightTagsInput =
            createTagsInput->Belt.Array.map(tag =>
              {
                "id": Some(Uuid.makeV4()),
                "highlightId": highlight##id,
                "tagId": tag##id,
              }
            );
          (createTagsInput, createHighlightTagsInput);
        };

        let variables =
          UpdateHighlightMutation.makeVariables(
            ~updateHighlightInput={
              "id": highlight##id,
              "text": textState->Js.Option.some,
              "createdAt": None,
              "note": None,
              "highlightScreenshotId": None,
              "owner": None,
            },
            ~createTagsInput,
            ~createHighlightTagsInput,
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
  let handleTagsChange = s => setTagsState(_ => s);

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
      <TextInput.Highlight
        onTextChange=handleTextChange
        textValue=textState
        tagsValue=tagsState
        onTagsChange=handleTagsChange
      />
    </div>
  </div>;
};
