let filterProps = [%raw
  {|
    function (p) {
      return Object.keys(p).reduce((memo, key) => {
        if (!['contentState', 'decoratedText', 'blockKey', 'entityKey', 'offsetKey'].includes(key)) {
          memo[key] = p[key]
        }
        return memo
      }, {})
    }
  |}
];

let findWithRegex = (~matchIdx=0, regex, contentBlock, callback) => {
  let text = contentBlock->Draft.Block.getText;
  let break = ref(false);
  while (! break^) {
    switch (regex->Js.Re.exec_(text)) {
    | None => break := true
    | Some(result) =>
      let _ =
        result
        ->Js.Re.captures
        ->Belt.Array.get(matchIdx)
        ->Belt.Option.flatMap(Js.Nullable.toOption)
        ->Belt.Option.map(match_ => {
            let start = result->Js.Re.index;
            let offset =
              matchIdx === 0
                ? 0
                : result
                  ->Js.Re.captures
                  ->Belt.Array.get(0)
                  ->Belt.Option.flatMap(Js.Nullable.toOption)
                  ->Belt.Option.map(fullMatch =>
                      Js.String2.indexOf(fullMatch, match_)
                    )
                  ->Belt.Option.getWithDefault(0);

            callback(.
              start + offset,
              start + offset + Js.String.length(match_),
            );
          });
      ();
    };
  };
};
