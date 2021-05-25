type router = {
  query: Js.Json.t,
  pathname: string,
  asPath: string,
  route: string
};

type path('a) = {params: 'a};
type staticPaths('a) = {
  paths: array(path('a)),
  fallback: bool,
};

type getStaticPropsContext('a) = {params: 'a};

type getStaticPropsResult('a) = {props: 'a};

module Head = {
  [@bs.module "next/head"] [@react.component]
  external make: (~children: React.element) => React.element = "default";
};

module Router = {
  [@bs.module "next/router"] external useRouter: unit => router = "useRouter";

  type replaceOptions = {
    shallow: bool
  };

  [@bs.module "next/router"] [@bs.scope "default"]
  external replace: string => unit = "replace";
  [@bs.module "next/router"] [@bs.scope "default"]
  external replaceWithAs: (string, string) => unit = "replace";
  [@bs.module "next/router"] [@bs.scope "default"]
  external replaceWithOptions: (string, string, replaceOptions) => unit = "replace";

  [@bs.module "next/router"] [@bs.scope "default"]
  external back: unit => unit = "back";

  [@bs.module "next/router"] [@bs.scope "default"]
  external push: string => unit = "push";
  [@bs.module "next/router"] [@bs.scope "default"]
  external pushWithAs: (string, string) => unit = "push";
};

module Link = {
  [@bs.module "next/link"] [@react.component]
  external make:
    (
      ~href: string,
      ~_as: string=?,
      ~passHref: bool=?,
      ~prefetch: bool=?,
      ~replace: bool=?,
      ~scroll: bool=?,
      ~children: React.element=?
    ) =>
    React.element =
    "default";
};

