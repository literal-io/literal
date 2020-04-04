open Styles;

let emptyContentState = () =>
  Draft.convertFromRaw({
    entityMap: Js.Dict.empty(),
    blocks: [|
      {text: "", key: "editor", type_: "unstyled", entityRanges: [||]},
    |],
  });

[@react.component]
let make =
    (
      ~className=?,
      ~contentState=emptyContentState(),
      ~customStyleMap=?,
      ~decorator=?,
      ~editorKey=?,
      ~onGetBlockClassName=?,
    ) => {
  let (editorState, setEditorState) =
    React.useState(() =>
      Draft.(editorStateClass->makeWithContent(contentState, decorator))
    );

  let handleChange = nextEditorState => setEditorState(_ => nextEditorState);

  <div ?className>
    <Draft.Editor
      ?editorKey
      ?customStyleMap
      blockStyleFn=?onGetBlockClassName
      editorState
      onChange=handleChange
      placeholder="Thoughts..."
    />
  </div>;
};
