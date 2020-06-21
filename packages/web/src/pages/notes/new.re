let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

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

  <Provider
    render={(~rehydrated) =>
      switch (authentication, highlightId) {
      | (Loading, _) => <Loading />
      | (Unauthenticated, _) => <Loading />
      | _ when !rehydrated => <Loading />
      | (Authenticated(currentUser), Some(highlightId)) =>
        <QueryRenderers_NewNoteFromShare highlightId currentUser />
      | (Authenticated(currentUser), None) =>
        <QueryRenderers_NewNote currentUser />
      }
    }
  />;
};
