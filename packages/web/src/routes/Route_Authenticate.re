open Styles;

[@react.component]
let default = () => {
  let authentication = Hooks_CurrentUserInfo.use();
  let (isAuthenticating, setIsAuthenticating) = React.useState(() => false);
  let router = Next.Router.useRouter();
  let searchParams =
    router.asPath
    ->Js.String2.split("?")
    ->Belt.Array.get(1)
    ->Belt.Option.getWithDefault("")
    ->Webapi.Url.URLSearchParams.make;

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (
            authentication,
            searchParams |> Webapi.Url.URLSearchParams.get("forResult"),
          ) {
          | (Authenticated(currentUser), None) =>
            setIsAuthenticating(_ => false);
            Routes.CreatorsIdAnnotationCollectionsId.(
              Next.Router.replaceWithAs(
                staticPath,
                path(
                  ~creatorUsername=currentUser.username,
                  ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                ),
              )
            );
          | (Authenticated(currentUser), Some(_)) =>
            setIsAuthenticating(_ => false);
            let _ =
              Webview.(
                postMessage(WebEvent.make(~type_="ACTIVITY_FINISH", ()))
              );
            ();
          | _ => ()
          };
        None;
      },
      [|authentication|],
    );

  let handleAuthenticateGoogle = () => {
    setIsAuthenticating(_ => true);
    let didPostMessage =
      Webview.(postMessage(WebEvent.make(~type_="AUTH_SIGN_IN", ())));

    let _ =
      if (!didPostMessage) {
        AwsAmplify.Auth.(
          federatedSignInWithOptions(
            inst,
            {provider: "Google", customState: None},
          )
        );
      };
    ();
  };

  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "justify-end",
      "py-6",
      "px-6",
    ])}>
    <div
      className={cn(["flex-auto", "flex", "items-center", "justify-center"])}>
      <Svg
        icon=Svg.logo
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "w-20", "h-20"])}
      />
    </div>
    <MaterialUi.Button
      onClick={_ => {
        let _ =
          Service_Analytics.(
            track(
              Click({
                action: "authenticate",
                label: Some("Sign In With Google"),
              }),
            )
          );
        handleAuthenticateGoogle();
      }}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-75"]),
        },
      }
      size=`Large
      classes={MaterialUi.Button.Classes.make(
        ~root=cn([Cn.on("py-4", !isAuthenticating)]),
        ~label=
          cn([
            "text-white",
            "font-sans",
            "font-bold",
            "text-lg",
            "leading-none",
            "italic",
          ]),
        ~outlined=cn(["border-white"]),
        (),
      )}
      variant=`Outlined>
      {isAuthenticating
         ? <MaterialUi.CircularProgress
             variant=`Indeterminate
             size={MaterialUi.CircularProgress.Size.int(36)}
             classes={MaterialUi.CircularProgress.Classes.make(
               ~colorPrimary=Cn.fromList(["text-white"]),
               (),
             )}
           />
         : React.string("Sign In With Google")}
    </MaterialUi.Button>
  </div>;
};

let page = "authenticate.js";
