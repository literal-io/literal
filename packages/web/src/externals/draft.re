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

[@bs.send] external getText: block => string = "getText";

type decoratorComponent('a) = Js.t({..} as 'a) => React.element;
type decoratorInput('a) = {
  strategy: (block, (. int, int) => unit, contentState) => unit,
  component: decoratorComponent('a),
};

type compositeDecorator;

type rawEditorState = {
  entityMap: Js.Dict.t(entity),
  blocks: array(block),
};

[@bs.module "draft-js"]
external editorStateClass: editorStateClass = "EditorState";

[@bs.module "draft-js"]
external convertFromRaw: rawEditorState => contentState = "convertFromRaw";

[@bs.send]
external createEmpty: editorStateClass => editorState = "createEmpty";
[@bs.send]
external makeWithContent:
  (editorStateClass, contentState, compositeDecorator) => editorState =
  "createWithContent";

[@bs.module "draft-js"] [@bs.new]
external makeCompositeDecorator:
  array(decoratorInput('a)) => compositeDecorator =
  "CompositeDecorator";

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
