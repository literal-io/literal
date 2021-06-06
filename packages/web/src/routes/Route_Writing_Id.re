type param = {id: string};

module Data = {
  [@decco.decode]
  type item = {
    title: string,
    subtitle: string,
    filename: string,
  };

  [@decco.decode]
  type t = array(item);

  let decode = t_decode;
};

[@react.component]
let default = (~html, ~title, ~subtitle) =>
  <>
    <Head>
      {Head.makeMeta(~title=title ++ " / Literal", ~description=subtitle, ())}
    </Head>
    <header
      style={ReactDOM.Style.make(~maxWidth="750px", ())}
      className={Cn.fromList([
        "mt-12",
        "flex",
        "flex-col",
        "px-6",
        "sm:px-12",
      ])}>
      <Next.Link _as={Routes.Index.path()} href=Routes.Index.staticPath>
        <a>
          <Svg
            icon=Svg.logo
            className={Cn.fromList([
              "pointer-events-none",
              "w-12",
              "h-12",
              "block",
            ])}
          />
        </a>
      </Next.Link>
      <h1
        className={Cn.fromList([
          "font-serif",
          "text-lightPrimary",
          "text-xl",
          "pt-24",
          "pb-12",
        ])}>
        {React.string("Writing")}
      </h1>
      <h1
        className={Cn.fromList([
          "font-serif",
          "text-lightPrimary",
          "text-2xl",
        ])}>
        {React.string(title)}
      </h1>
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightSecondary",
          "text-base",
          "mb-16"
        ])}>
        {React.string(subtitle)}
      </p>
      <hr
        className={Cn.fromList([
          "border-t",
          "border-dotted",
          "border-lightDisabled",
        ])}
      />
    </header>
    <main
      className={Cn.fromList(["px-6", "sm:px-12", "mt-16"])}
      style={ReactDOM.Style.make(~maxWidth="750px", ())}>
      <Markdown html />
    </main>
  </>;

let getStaticProps =
    (ctx: Next.getStaticPropsContext(param))
    : Next.getStaticPropsResult({
        .
        "html": string,
        "title": string,
        "subtitle": string,
      }) => {
  let htmlForId =
    Js.Dict.fromList([
      (
        "prologue",
        [%raw "require('../static/markdown/writing/prologue.md')"]: string,
      ),
    ]);
  let data: Js.Json.t = [%raw "require('../static/json/writing.json')"];

  switch (htmlForId->Js.Dict.get(ctx.params.id), Data.decode(data)) {
  | (Some(html), Belt.Result.Ok(data)) =>
    switch (
      data->Belt.Array.getBy(({filename}) => filename === ctx.params.id)
    ) {
    | Some({title, subtitle}) => {
        props: {
          "html": html,
          "title": title,
          "subtitle": subtitle,
        },
      }
    | None =>
      Js.Exn.raiseError("Unable to find metadata for id: " ++ ctx.params.id)
    }
  | (_, Belt.Result.Error(e)) =>
    Js.Exn.raiseError("Unable to decode data: " ++ e.message)
  | (None, _) =>
    Js.Exn.raiseError("Unable to find html for id: " ++ ctx.params.id)
  };
};

let getStaticPaths = (): Next.staticPaths(param) => {
  let data: Js.Json.t = [%raw "require('../static/json/writing.json')"];
  switch (Data.decode(data)) {
  | Belt.Result.Ok(items) => {
      paths:
        items->Belt.Array.map(({filename}) =>
          ({
             params: {
               id: filename,
             },
           }: Next.path(param))
        ),
      fallback: false,
    }
  | Belt.Result.Error(e) =>
    Js.Exn.raiseError("Unable to decode data: " ++ e.message)
  };
};

let page = "writing/[id].js";
