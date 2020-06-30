let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

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

  <>
    <Provider
      render={(~rehydrated) =>
        <>
          {switch (
             authentication,
             Routes.New.params_decode(router.Next.query),
           ) {
           | (Unauthenticated, _) => <Loading />
           | (_, Ok({id: Some(highlightId)}))
               when Js.String.length(highlightId) > 0 =>
             <QueryRenderers_NewNoteFromShare
               highlightId
               authentication
               rehydrated
             />
           | (Loading, _) => <Loading />
           | _ when !rehydrated => <Loading />
           | (Authenticated(currentUser), Ok({initialPhaseState})) =>
             <QueryRenderers_NewNote currentUser ?initialPhaseState />
           | (Authenticated(currentUser), _) =>
             <QueryRenderers_NewNote currentUser />
           }}
          <Alert query={router.query} />
        </>
      }
    />
  </>;
};
