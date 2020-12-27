[@react.component]
let default = (~html) => <Policy html />;

let page = "policies/[id].js";
type param = {id: string};

let getStaticProps =
    (ctx: Next.getStaticPropsContext(param))
    : Next.getStaticPropsResult({. "html": string}) => {
  let privacy: string = [%raw
    "require('../static/markdown/privacy-policy.md')"
  ];
  let subprocessors: string = [%raw
    "require('../static/markdown/subprocessors.md')"
  ];

  {
    props: {
      "html":
        switch (ctx.params.id) {
        | "privacy" => privacy
        | "subprocessors" => subprocessors
        | _ => ""
        },
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
