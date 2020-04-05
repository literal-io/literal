type router = {query: Js.Json.t};

[@bs.module "next/router"] external useRouter: unit => router = "useRouter";

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
