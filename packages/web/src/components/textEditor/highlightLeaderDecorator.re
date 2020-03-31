let highlightLeaderComponent = props => {
  ReasonReact.cloneElement(
    <span className="text-black" />,
    ~props=DecoratorUtils.filterProps(props),
    [|props##children|],
  );
};

let highlightRegex = [%re "/^\>/gm"];

let decoratorInput =
  Draft.{
    strategy: (contentBlock, callback, contentState) => {
      DecoratorUtils.findWithRegex(highlightRegex, contentBlock, callback);
    },
    component: highlightLeaderComponent,
  };
