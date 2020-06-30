[@react.component]
let make = (~path, ~query) => {
  let _ =
    React.useEffect0(_ => {
      let _ = Next.Router.(replaceWithOptions({pathname: path, query}));
      None;
    });
  React.null;
};
