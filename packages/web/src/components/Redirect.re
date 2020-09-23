[@react.component]
let make = (~path, ~staticPath, ~children) => {
  let _ =
    React.useEffect0(_ => {
      let _ = Next.Router.(replaceWithAs(staticPath, path));
      None;
    });
  children;
};
