[@decco]
type routeParams = {id: string};

[@react.component]
let default = () => {
  let router = Next.Router.useRouter();

  let highlightId =
    switch (routeParams_decode(router.Next.query)) {
    | Ok(p) => Some(p.id)
    | _ => None
    };

  let handleHighlightIdChange = highlightId =>
    Next.Router.(
      replaceWithOptions({
        pathname: router.pathname,
        query: routeParams_encode({id: highlightId}),
      })
    );

  <QueryRenderers_Notes
    highlightId
    onHighlightIdChange=handleHighlightIdChange
  />;
};
