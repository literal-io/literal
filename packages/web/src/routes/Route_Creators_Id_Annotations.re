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

  let annotationId =
    switch (routeParams_decode(router.Next.query)) {
    | Ok(p) => Some(p.id)
    | _ => None
    };

  let handleAnnotationIdChange = annotationId =>
    Next.Router.(
      replaceWithOptions({
        pathname: router.pathname,
        query: routeParams_encode({id: annotationId}),
      })
    );

  <Provider
    render={(~rehydrated) =>
      switch (authentication) {
      | Unauthenticated => <Loading />
      | Loading | Authenticated(_) =>
        <QueryRenderers_Notes
          annotationId
          onAnnotationIdChange=handleAnnotationIdChange
          authentication
          rehydrated
        />
      }
    }
  />;
};

let page = "creators/[creatorUsername]/annotations.js"
