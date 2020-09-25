type router = {
  query: Js.Json.t,
  pathname: string,
  asPath: string,
};

module Router = {
  [@bs.module "next/router"] external useRouter: unit => router = "useRouter";

  type urlOptions = {
    pathname: string,
    query: Js.Json.t,
  };

  [@bs.module "next/router"] [@bs.scope "default"]
  external replace: string => unit = "replace";
  [@bs.module "next/router"] [@bs.scope "default"]
  external replaceWithAs: (string, string) => unit = "replace";
  [@bs.module "next/router"] [@bs.scope "default"]
  external replaceWithOptions: urlOptions => unit = "replace";

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
      ~children: React.element
    ) =>
    React.element =
    "default";
};
