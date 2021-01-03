let makeMeta =
    (
      ~title="Literal",
      ~description="Literal is a textual annotation management system. The act of annotation is only the beginning. Leverage your highlights to enhance your reading experience.",
      ~image=Constants.apiOrigin ++ "/logo-opaque-128.png",
      (),
    ) =>
  <>
    <title key="title"> {React.string(title)} </title>
    <meta key="description" property="description" content=description />
    <meta key="og:title" property="og:title" content=title />
    <meta key="og:description" property="og:description" content=description />
    <meta key="og:image" property="og:image" content=image />
  </>;

[@react.component]
let make = (~children=React.null) =>
  <Next.Head>
    <meta
      key="viewport"
      name="viewport"
      content="initial-scale=1.0, width=device-width"
    />
    <meta key="twitter:card" name="twitter:card" content="summary" />
    <link key="favicon" rel="icon" type_="image/png" href="/favicon-32.png" />
    <meta key="charset" charSet="UTF-8" />
    {makeMeta()}
    children
  </Next.Head>;
