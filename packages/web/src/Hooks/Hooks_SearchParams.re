let use = parser => {
  let router = Next.Router.useRouter();

  router.asPath
  ->Js.String2.split("?")
  ->Belt.Array.get(1)
  ->Belt.Option.getWithDefault("")
  ->Webapi.Url.URLSearchParams.make
  ->parser;
};
