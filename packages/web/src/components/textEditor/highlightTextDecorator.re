let highlightTextComponent = props => {
  ReasonReact.cloneElement(
    <HighlightText />,
    ~props=DecoratorUtils.filterProps(props),
    [|props##children|],
  );
};

let decoratorInput =
  Draft.Decorator.{
    strategy: (contentBlock, callback, _contentState) => {
      callback(. 0, contentBlock->Draft.Block.getText->Js.String2.length);
    },
    component: highlightTextComponent,
  };
