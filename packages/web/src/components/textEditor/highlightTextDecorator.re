let highlightTextComponent = props => {
  ReasonReact.cloneElement(
    <span className="bg-white text-black font-serif text-lg leading-snug" />,
    ~props=DecoratorUtils.filterProps(props),
    [|props##children|],
  );
};

let highlightRegex = [%re "/^\>(.+\\n?)/gm"];
let decoratorInput =
  Draft.{
    strategy: (contentBlock, callback, contentState) => {
      DecoratorUtils.findWithRegex(
        ~matchIdx=1,
        highlightRegex, 
        contentBlock,
        callback
      );
    },
    component: highlightTextComponent,
  };
