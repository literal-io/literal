type editorStateClass;
type editorState;

type contentState;

type entityRange;
type entity;

type block = {
  text: string,
  key: string,
  [@bs.as "type"]
  type_: string,
  entityRanges: array(entityRange),
};
type rawEditorState = {
  entityMap: Js.Dict.t(entity),
  blocks: array(block),
};

[@bs.module "draft-js"]
external editorStateClass: editorStateClass = "EditorState";

[@bs.send]
external createEmpty: editorStateClass => editorState = "createEmpty";
[@bs.send]
external createWithContent: (editorStateClass, contentState) => editorState =
  "createWithContent";

[@bs.module "draft-js"]
external convertFromRaw: rawEditorState => contentState = "convertFromRaw";

module Editor = {
  [@bs.module "draft-js"] [@react.component]
  external make:
    (
      ~editorState: editorState,
      ~editorKey: string,
      ~onChange: editorState => unit,
      ~placeholder: string
    ) =>
    React.element =
    "Editor";
};
