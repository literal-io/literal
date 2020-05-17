[@decco]
type routeParams = {id: string};

[@react.component]
let default = () => {
  let router = Next.Router.useRouter();
  let authentication = CurrentUserInfo.use();

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (authentication) {
          | Unauthenticated => Next.Router.replace("/authenticate")
          | _ => ()
          };
        None;
      },
      [|authentication|],
    );

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

  switch (authentication) {
  | Loading
  | Unauthenticated => <Loading />
  | Authenticated(currentUser) =>
    <QueryRenderers_Notes
      highlightId
      onHighlightIdChange=handleHighlightIdChange
      currentUser
    />
  };
};
