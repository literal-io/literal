open Styles;
open Containers_NoteEditor_GraphQL;
let styles = [%raw "require('./Containers_NoteEditor.module.css')"];

type tagState = {
  commits:
    array({
      .
      "id": string,
      "text": string,
    }),
  partial: string,
  filterResults:
    option(
      array({
        .
        "text": string,
        "id": string,
      }),
    ),
};

[@react.component]
let make = (~highlightFragment as highlight, ~isActive=true, ~currentUser) => {
  let (updateHighlightMutation, _s, _f) =
    ApolloHooks.useMutation(UpdateHighlightMutation.definition);

  let (textState, setTextState) = React.useState(() => {highlight##text});
  let (tagsState, setTagsState) =
    React.useState(() =>
      highlight##tags
      ->Belt.Option.flatMap(t => t##items)
      ->Belt.Option.map(t =>
          {
            partial: "",
            commits:
              t->Belt.Array.keepMap(t => t)->Belt.Array.map(t => t##tag),
            filterResults: None,
          }
        )
      ->Belt.Option.getWithDefault({
          partial: "",
          commits: [||],
          filterResults: None,
        })
    );
  let _ =
    React.useEffect2(
      () => {/**
     * FIXME - onValueChange callback
     */ None},
      (textState.commits, tagsState),
    );

  let handleTextChange = s => setTextState(_ => s);
  let handleTagsChange = (s: TextInput_Tags.Value.t) =>
    setTagsState(tagsState => {
      let updatedCommits =
        s.commits
        ->Belt.Array.map(text => {
            switch (
              Belt.Array.getBy(tagsState.commits, tag => tag##text === text),
              tagsState.filterResults
              ->Belt.Option.flatMap(r =>
                  r->Belt.Array.getBy(tag => tag##text === text)
                ),
            ) {
            | (Some(tag), _) => tag
            | (_, Some(tag)) => tag
            | _ => {"id": Uuid.makeV4(), "text": text}
            }
          });

      {
        partial: s.partial,
        commits: updatedCommits,
        filterResults: tagsState.filterResults,
      };
    });

  let handleTagsFilterResults = s =>
    setTagsState(tagsState => {...tagsState, filterResults: s});

  let handleTagsFilterClicked = tag =>
    setTagsState(tagsState =>
      {
        partial: "",
        filterResults: tagsState.filterResults,
        commits: Belt.Array.concat(tagsState.commits, [|tag|]),
      }
    );

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
        className={cn([styles##underline])}
        onTextChange=handleTextChange
        textValue=textState
        tagsValue={
          TextInput_Tags.Value.partial: tagsState.partial,
          commits: tagsState.commits->Belt.Array.map(t => t##text),
        }
        onTagsChange=handleTagsChange
      />
      {isActive && Js.String.length(tagsState.partial) > 0
         ? <QueryRenderers_TagsFilter
             text={tagsState.partial}
             onTagResults=handleTagsFilterResults
             onTagClicked=handleTagsFilterClicked
           />
         : React.null}
    </div>
  </div>;
};
