let emptyContentState = () =>
  Draft.ContentState.convertFromRaw({
    entityMap: Js.Dict.empty(),
    blocks: [|
      {text: "", key: "editor", type_: "unstyled", entityRanges: [||]},
    |],
  });

[@react.component]
let make =
    (
      ~editorState,
      ~onChange,
      ~className=?,
      ~customStyleMap=?,
      ~editorKey=?,
      ~onGetBlockClassName=?,
    ) => {
  <div ?className>
    <Draft.Editor
      ?editorKey
      ?customStyleMap
      blockStyleFn=?onGetBlockClassName
      editorState
      onChange
      placeholder="Thoughts..."
    />
  </div>;
};
