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
let default = (~html) =>
  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "overflow-y-auto",
      "bg-backgroundGray",
    ])}>
    <div
      className={Cn.fromList([
        "p-8",
        "bg-black",
        "max-w-lg",
        "m-auto",
        "w-full",
        "min-h-full",
      ])}>
      <header
        className={Cn.fromList([
          "mb-12",
          "flex",
          "flex-row",
          "justify-between",
          "items-center",
        ])}>
        <h1
          className={Cn.fromList([
            "font-serif",
            "text-lightPrimary",
            "text-xl",
          ])}>
          {React.string("Writing")}
        </h1>
        <Next.Link _as={Routes.Index.path()} href=Routes.Index.staticPath>
          <a>
            <Svg
              icon=Svg.logo
              placeholderViewBox="0 0 24 24"
              className={Cn.fromList([
                "pointer-events-none",
                "w-12",
                "h-12",
                "block",
              ])}
            />
          </a>
        </Next.Link>
      </header>
      <main />
    </div>
  </div>;

let getStaticProps =
    (ctx: Next.getStaticPropsContext(param))
    : Next.getStaticPropsResult({. "html": string}) => {
  let requireHtml: string => string = [%raw
    "function (path) { return require(path) }"
  ];

  {
    props: {
      "html": requireHtml("../static/markdown/blog" ++ ctx.params.id),
    },
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
