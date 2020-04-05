type entityRange;
type entity;

module Block = {
  [@bs.deriving accessors]
  type t = {
    text: string,
    key: string,
    [@bs.as "type"]
    type_: string,
    entityRanges: array(entityRange),
  };

  [@bs.send] external getText: t => string = "getText";
};

module ContentState = {
  type t;

  [@bs.deriving accessors]
  type raw = {
    entityMap: Js.Dict.t(entity),
    blocks: array(Block.t),
  };

  [@bs.module "draft-js"] [@bs.scope "ContentState"]
  external createFromText: string => t = "createFromText";

  [@bs.module "draft-js"] external convertFromRaw: raw => t = "convertFromRaw";

  [@bs.module "draft-js"] external convertToRaw: t => raw = "convertToRaw";
};

module Decorator = {
  type component('a) = Js.t({..} as 'a) => React.element;
  type input('a) = {
    strategy: (Block.t, (. int, int) => unit, ContentState.t) => unit,
    component: component('a),
  };
  type composite;

  [@bs.module "draft-js"] [@bs.new]
  external makeComposite: array(input('a)) => composite =
    "CompositeDecorator";
};

module EditorState = {
  type t;

  [@bs.module "draft-js"] [@bs.scope "EditorState"]
  external createEmpty: unit => t = "createEmpty";

  [@bs.module "draft-js"] [@bs.scope "EditorState"]
  external makeWithContent: (ContentState.t, option(Decorator.composite)) => t =
    "createWithContent";

  [@bs.send]
  external getCurrentContent: t => ContentState.t = "getCurrentContent";
};

module Editor = {
  [@bs.module "draft-js"] [@react.component]
  external make:
    (
      ~editorState: EditorState.t,
      ~editorKey: option(string),
      ~blockStyleFn: option(Block.t => string),
      ~customStyleMap: option(Js.Dict.t(ReactDOMRe.Style.t)),
      ~onChange: EditorState.t => unit,
      ~placeholder: string
    ) =>
    React.element =
    "Editor";
};
