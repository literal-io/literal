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
          | Unauthenticated => Next.Router.replace(Routes.Authenticate.path())
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
             Routes.CreatorsIdAnnotationsNew.queryParams_decode(
               router.Next.query,
             ),
           ) {
           | (Unauthenticated, _) => <Loading />
           | (_, Ok({id: Some(annotationId)}))
               when Js.String.length(annotationId) > 0 =>
             <QueryRenderers_NewNoteFromShare
               annotationId
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

let page = "creators/[creatorUsername]/annotations/new.js";
