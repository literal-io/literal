open Styles;
open Containers_NoteEditor_Base_Types;

let styles = [%raw "require('./Containers_NoteEditor_Base.module.css')"];

[@react.component]
let make =
    (
      ~annotationFragment as annotation=?,
      ~isActive=true,
      ~onChange,
      ~currentUser,
      ~autoFocus=?,
      ~placeholder=?,
    ) => {
  let (textState, setTextState) =
    React.useState(() =>
      annotation
      ->Belt.Option.flatMap(a =>
          a##target
          ->Belt.Array.getBy(target =>
              switch (target) {
              | `TextualTarget(_) => true
              | `ExternalTarget(_) => false
              }
            )
        )
      ->Belt.Option.flatMap(target =>
          switch (target) {
          | `TextualTarget(target) => Some(target##value)
          | `ExternalTarget(_) => None
          }
        )
      ->Belt.Option.getWithDefault("")
    );

  let (tagsState, setTagsState) =
    React.useState(() =>
      annotation
      ->Belt.Option.flatMap(a => a##body)
      ->Belt.Option.map(bodies => {
          let commits =
            bodies->Belt.Array.keepMap(body =>
              switch (body) {
              | `Nonexhaustive => None
              | `TextualBody(body) =>
                Lib_GraphQL.Annotation.isBodyTag(body)
                  ? Some({text: body##value, id: None}) : None
              }
            );
          {partial: "", commits, filterResults: [||]};
        })
      ->Belt.Option.getWithDefault({
          partial: "",
          commits: [||],
          filterResults: [||],
        })
    );
  let _ =
    React.useEffect2(
      () => {
        let _ = onChange({tags: tagsState.commits, text: textState});
        None;
      },
      (
        textState,
        tagsState.commits
        ->Belt.Array.map(c => c.text)
        ->Js.Array2.joinWith("-"),
      ),
    );

  let handleTextChange = s => setTextState(_ => s);
  let handleTagsChange = (s: TextInput_Tags.Value.t) =>
    setTagsState(tagsState => {
      let updatedCommits =
        s.commits
        ->Belt.Array.map(text => {
            switch (
              Belt.Array.getBy(tagsState.commits, tag => tag.text === text),
              tagsState.filterResults
              ->Belt.Array.getBy(tag => tag.text === text),
            ) {
            | (Some(tag), _) => tag
            | (_, Some(tag)) => tag
            | _ => {id: None, text}
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
          commits: tagsState.commits->Belt.Array.map(t => t.text),
        }
        onTagsChange=handleTagsChange
        ?placeholder
        ?autoFocus
      />
      {isActive && Js.String.length(tagsState.partial) > 0
         ? <QueryRenderers_TagsFilter
             tagsState={tagsState}
             onTagResults=handleTagsFilterResults
             onTagClicked=handleTagsFilterClicked
             currentUser
           />
         : React.null}
    </div>
  </div>;
};
