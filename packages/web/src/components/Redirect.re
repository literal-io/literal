let encodeSearch = [%raw
  {|
  function encodeSearch(o) {
    return "?" + (new URLSearchParams(o)).toString()
  }
|}
];

[@react.component]
let make = (~path, ~staticPath, ~search=?, ~children) => {
  let _ =
    React.useEffect0(_ => {
      let stringifiedSearch =
        search
        ->Belt.Option.map(encodeSearch)
        ->Belt.Option.getWithDefault("");
      let _ =
        Next.Router.(replaceWithAs(staticPath, path ++ stringifiedSearch));
      None;
    });
  children;
};
