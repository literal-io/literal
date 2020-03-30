let highlightComponent = props => ReasonReact.cloneElement(
  <span className="bg-white" />,
  ~props,
  [|props##children|]
)

let findWithRegex = (regex, contentBlock, callback) => {
  let text = contentBlock->Draft.getText;
  let break = ref(false);
  while (! break^) {
    switch (regex->Js.Re.exec_(text)) {
    | None => break := true
    | Some(result) =>
      let _ =
        result
        ->Js.Re.captures
        ->Belt.Array.get(0)
        ->Belt.Option.flatMap(Js.Nullable.toOption)
        ->Belt.Option.map(match_ => {
            let start = result->Js.Re.index;
            callback(. start, start + Js.String.length(match_));
          });
      ();
    };
  };
};

let highlightRegex = [%re "/\>.+\\n?/g"];
let decoratorInput =
  Draft.{
    strategy: (contentBlock, callback, contentState) => {
      findWithRegex(highlightRegex, contentBlock, callback);
    },
    component: highlightComponent
  };
