type router = {query: Js.Json.t};

module Router = {
  [@bs.module "next/router"] external useRouter: unit => router = "useRouter";
  [@bs.module "next/router"] [@bs.scope "default"]
  external replace: string => unit = "replace";

  [@bs.module "next/router"] [@bs.scope "default"]
  external back: unit => unit = "back";
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
