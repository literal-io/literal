type editorStateClass;
type editorState;

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

module ContentState = {
  type t;

  [@bs.module "draft-js"] [@bs.scope "ContentState"]
  external createFromText: string => t = "createFromText";
};

type decoratorComponent('a) = Js.t({..} as 'a) => React.element;
type decoratorInput('a) = {
  strategy: (block, (. int, int) => unit, ContentState.t) => unit,
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
external convertFromRaw: rawEditorState => ContentState.t = "convertFromRaw";

/** editorStateClass **/
[@bs.send]
external createEmpty: editorStateClass => editorState = "createEmpty";
[@bs.send]
external makeWithContent:
  (editorStateClass, ContentState.t, option(compositeDecorator)) =>
  editorState =
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
      ~editorKey: option(string),
      ~blockStyleFn: option(block => string),
      ~customStyleMap: option(Js.Dict.t(ReactDOMRe.Style.t)),
      ~onChange: editorState => unit,
      ~placeholder: string
    ) =>
    React.element =
    "Editor";
};
