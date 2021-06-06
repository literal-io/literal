[@react.component]
let default = (~html, ~title, ~description) =>
  <>
    <Head>
      {Head.makeMeta(~title=title ++ " / Literal", ~description, ())}
    </Head>
    <header
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
    </header>
    <main
      className={Cn.fromList(["px-6", "sm:px-12", "py-24"])}
      style={ReactDOM.Style.make(~maxWidth="750px", ())}>
      <Markdown html />
    </main>
  </>;

let page = "policies/[id].js";
type param = {id: string};

let getStaticProps =
    (ctx: Next.getStaticPropsContext(param))
    : Next.getStaticPropsResult({
        .
        "html": string,
        "title": string,
        "description": string,
      }) => {
  let privacy: string = [%raw
    "require('../static/markdown/privacy-policy.md')"
  ];
  let subprocessors: string = [%raw
    "require('../static/markdown/subprocessors.md')"
  ];

  {
    props: {
      switch (ctx.params.id) {
      | "privacy" => {
          "html": privacy,
          "title": "Privacy Policy",
          "description": "What data we collect and why.",
        }
      | "subprocessors" => {
          "html": subprocessors,
          "title": "Subprocessors",
          "description": "The list of third-party subprocessors we utilize.",
        }
      | _ => Js.Exn.raiseError("Unexpected route id: " ++ ctx.params.id)
      };
    },
  };
};

let getStaticPaths = (): Next.staticPaths(param) => {
  {
    paths: [|{
               params: {
                 id: "privacy",
               },
             }, {
                  params: {
                    id: "subprocessors",
                  },
                }|],
    fallback: false,
  };
};
