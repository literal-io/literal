open Styles;

let highlightTextComponent = props => {
  ReasonReact.cloneElement(
    <span
      className={cn([
        "bg-white",
        "text-black",
        "font-serif",
        "text-lg",
        "leading-relaxed",
        "box-decoration-break-clone",
        "px-1",
      ])}
      style={ReactDOMRe.Style.make(
        ~paddingTop="0.125rem",
        ~paddingBottom="0.125rem",
        (),
      )}
    />,
    ~props=DecoratorUtils.filterProps(props),
    [|props##children|],
  );
};

let decoratorInput =
  Draft.{
    strategy: (contentBlock, callback, _contentState) => {
      callback(. 0, contentBlock->Draft.getText->Js.String2.length);
    },
    component: highlightTextComponent,
  };
