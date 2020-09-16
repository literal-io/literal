open Styles;
open Containers_AnnotationEditor_Base_Types;

let styles = [%raw "require('./Containers_AnnotationEditor_Base.module.css')"];

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
                  ? Some({text: body##value, id: body##id}) : None
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
  let tagsInputRef = React.useRef(Js.Nullable.null);

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
        ->Belt.Array.map(({text}) => {
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

  let handleTagsFilterClicked = tag => {
    let _ =
      setTagsState(tagsState =>
        {
          partial: "",
          filterResults: tagsState.filterResults,
          commits: Belt.Array.concat(tagsState.commits, [|tag|]),
        }
      );
    let _ =
      Js.Global.setTimeout(
        () => {
          let _ =
            switch (tagsInputRef.current->Js.Nullable.toOption) {
            | Some(inputElem) =>
              let _ =
                inputElem
                ->Webapi.Dom.Element.unsafeAsHtmlElement
                ->Webapi.Dom.HtmlElement.focus;
              ();
            | None => ()
            };
          ();
        },
        0,
      );
    ();
  };

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
      <TextInput.Annotation
        className={cn([styles##underline])}
        onTextChange=handleTextChange
        textValue=textState
        tagsValue={TextInput_Tags.Value.fromTagsState(
          ~state=tagsState,
          ~currentUser,
        )}
        onTagsChange=handleTagsChange
        tagsInputRef
        ?placeholder
        ?autoFocus
      />
      {isActive && Js.String.length(tagsState.partial) > 0
         ? <QueryRenderers_TagsFilter
             tagsState
             onTagResults=handleTagsFilterResults
             onTagClicked=handleTagsFilterClicked
             currentUser
           />
         : <div className={Cn.fromList(["h-6"])} />}
    </div>
  </div>;
};
