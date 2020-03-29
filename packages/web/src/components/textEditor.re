open Styles;

let emptyContentState =
  Draft.convertFromRaw({
    entityMap: Js.Dict.empty(),
    blocks: [|
      {text: "", key: "editor", type_: "unstyled", entityRanges: [||]},
    |],
  });

[@react.component]
let make = () => {
  let (editorState, setEditorState) =
    React.useState(() =>
      Draft.(editorStateClass->createWithContent(emptyContentState))
    );

  let handleChange = nextEditorState => setEditorState(_ => nextEditorState);

  <div className={cn(["border-white", "border-b", "text-white"])}>
    <Draft.Editor
      editorKey="editor"
      editorState
      onChange=handleChange
      placeholder="Thoughts..."
    />
  </div>;
};
