open Styles;

let resetPasswordUrl =
  AwsAmplify.Config.(
    "https://"
    ++ Constants.awsAmplifyConfig->oauthGet->domainGet
    ++ "/forgotPassword?"
    ++ Webapi.Url.URLSearchParams.(
         makeWithArray([|
           ("client_id", Constants.awsAmplifyConfig->userPoolsWebClientIdGet),
           (
             "response_type",
             Constants.awsAmplifyConfig->oauthGet->responseTypeGet,
           ),
           (
             "scope",
             Constants.awsAmplifyConfig
             ->oauthGet
             ->scopeGet
             ->Js.Array2.joinWith("+"),
           ),
           (
             "redirect_uri",
             Constants.awsAmplifyConfig->oauthGet->redirectSignInGet,
           ),
         |])
         ->toString
         ->Js.Global.decodeURIComponent
       )
  );

[@react.component]
let default = () => {
  let authentication = Hooks_CurrentUserInfo.use();
  let (isAuthenticating, setIsAuthenticating) = React.useState(() => false);
  let (isMenuOpen, setIsMenuOpen) = React.useState(() => false);
  let menuIconButtonRef = React.useRef(Js.Nullable.null);
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
    setIsMenuOpen(_ => false);
    setIsAuthenticating(_ => true);
    let didPostMessage =
      Webview.(postMessage(WebEvent.make(~type_="AUTH_SIGN_IN_GOOGLE", ())));

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

  let handleToggleIsMenuOpen = () => setIsMenuOpen(open_ => !open_);

  <main
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
    <div className={Cn.fromList(["flex-row", "justify-end", "flex"])}>
      <MaterialUi.IconButton
        ref={menuIconButtonRef->ReactDOMRe.Ref.domRef}
        size=`Small
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => {handleToggleIsMenuOpen()}}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["p-0", "flex-initial", "flex-shrink-0"]),
          (),
        )}>
        <Svg
          className={Cn.fromList([
            "w-6",
            "h-6",
            "pointer-events-none",
            "opacity-75",
          ])}
          icon=Svg.more
        />
      </MaterialUi.IconButton>
      <MaterialUi.NoSsr>
        <MaterialUi.Menu
          anchorEl={MaterialUi_Types.Any(menuIconButtonRef.current)}
          anchorReference=`AnchorEl
          _open=isMenuOpen
          onClose={(_, _) => {handleToggleIsMenuOpen()}}>
          <MaterialUi.MenuItem
            classes={MaterialUi.MenuItem.Classes.make(
              ~root=
                Cn.fromList(["font-sans", "flex", "flex-1", "items-stretch"]),
              (),
            )}>
            <a
              href=resetPasswordUrl
              className={Cn.fromList(["flex", "flex-1", "items-center"])}>
              {React.string("Reset Password")}
            </a>
          </MaterialUi.MenuItem>
          <MaterialUi.MenuItem
            classes={MaterialUi.MenuItem.Classes.make(
              ~root=
                Cn.fromList(["font-sans", "flex", "flex-1", "items-stretch"]),
              (),
            )}>
            <Next.Link
              href=Routes.PolicyId.staticPath
              _as={Routes.PolicyId.path(~id="privacy")}
              passHref=true>
              <a className={Cn.fromList(["flex", "flex-1", "items-center"])}>
                {React.string("Privacy Policy")}
              </a>
            </Next.Link>
          </MaterialUi.MenuItem>
          <MaterialUi.MenuItem
            classes={MaterialUi.MenuItem.Classes.make(
              ~root=Cn.fromList(["font-sans"]),
              (),
            )}
            onClick={_ => handleAuthenticateGoogle()}>
            {React.string("Sign in with Google")}
          </MaterialUi.MenuItem>
        </MaterialUi.Menu>
      </MaterialUi.NoSsr>
    </div>
    <div
      className={cn([
        "flex-auto",
        "flex",
        "items-center",
        "justify-center",
        "flex-1",
      ])}>
      <Svg
        icon=Svg.logo
        className={cn(["pointer-events-none", "w-20", "h-20"])}
      />
    </div>
    <div className={Cn.fromList(["flex", "flex-row"])}>
      <Next.Link passHref=true href="/authenticate/sign-up">
        <MaterialUi.Button
          fullWidth=false
          classes={MaterialUi.Button.Classes.make(
            ~root=Cn.fromList(["flex-1"]),
            ~label=Cn.fromList(["font-sans"]),
            (),
          )}
          variant=`Contained
          size=`Large
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }>
          {React.string("Sign Up")}
        </MaterialUi.Button>
      </Next.Link>
      <Next.Link passHref=true href="/authenticate/sign-in">
        <MaterialUi.Button
          fullWidth=false
          classes={MaterialUi.Button.Classes.make(
            ~root=
              Cn.fromList([
                "ml-6",
                "border",
                "border-dotted",
                "border-lightSecondary",
                "flex-1",
              ]),
            ~label=Cn.fromList(["font-sans", "text-lightSecondary"]),
            (),
          )}
          variant=`Outlined
          size=`Large
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }>
          {React.string("Sign In")}
        </MaterialUi.Button>
      </Next.Link>
    </div>
  </main>;
};

let page = "authenticate.js";
