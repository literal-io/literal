type authenticationError =
  | UserNotFound
  | UserNotAuthorized
  | AuthenticationFailed;

[@react.component]
let default = () => {
  let Providers_Authentication.{user, setUser} =
    React.useContext(Providers_Authentication.authenticationContext);
  let Providers_ModalNavigation.{onNext} =
    React.useContext(Providers_ModalNavigation.context);
  let (isMenuOpen, setIsMenuOpen) = React.useState(() => false);
  let menuIconButtonRef = React.useRef(Js.Nullable.null);
  let (isAuthenticating, setIsAuthenticating) = React.useState(() => false);
  let (authenticationError, setAuthenticationError) =
    React.useState(() => None);

  let _ =
    React.useEffect1(
      () => {
        switch (user) {
        | SignedInUser({identityId}) =>
          onNext(() =>
            Routes.CreatorsIdAnnotationCollectionsId.(
              Next.Router.replaceWithAs(
                staticPath,
                path(
                  ~identityId,
                  ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                ),
              )
            )
          )
        | _ => ()
        };
        None;
      },
      [|user|],
    );

  let ((email, password), setAuthenticationValue) =
    React.useState(_ => ("", ""));

  let handleSignInResult = result => {
    switch (result->Belt.Option.map(Webview.WebEvent.AuthSignInResult.decode)) {
    | Some(Ok({error: None, user: Some(user)})) =>
      let authenticationUser =
        user->Providers_Authentication_User.makeFromAuthGetUserResult;
      let _ = setUser(authenticationUser);
      let _ = setIsAuthenticating(_ => false);
      let _ =
        switch (authenticationUser) {
        | SignedInUser({identityId}) =>
          onNext(() =>
            Next.Router.replaceWithAs(
              Routes.CreatorsIdAnnotationCollectionsId.staticPath,
              Routes.CreatorsIdAnnotationCollectionsId.path(
                ~identityId,
                ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
              ),
            )
          );
          ();
        | _ => setAuthenticationError(_ => Some(AuthenticationFailed))
        };
      ();
    | Some(Ok({error: Some("SIGN_IN_FAILED_USER_NOT_FOUND")})) =>
      setIsAuthenticating(_ => false);
      setAuthenticationError(_ => Some(UserNotFound));
    | Some(Ok({error: Some("SIGN_IN_FAILED_USER_NOT_AUTHORIZED")})) =>
      setIsAuthenticating(_ => false);
      setAuthenticationError(_ => Some(UserNotAuthorized));
    | Some(Ok({error: Some(_)}))
    | Some(Ok({user: None}))
    | Some(Error(_))
    | None =>
      setIsAuthenticating(_ => false);
      setAuthenticationError(_ => Some(AuthenticationFailed));
    };
  };
  let handleSubmit = () => {
    setAuthenticationError(_ => None);
    setIsAuthenticating(_ => true);
    let _ =
      Webview.postMessageForResult(
        Webview.WebEvent.make(
          ~type_="AUTH_SIGN_IN",
          ~data=
            Js.Json.object_(
              Js.Dict.fromList([
                ("email", email->Js.Json.string),
                ("password", password->Js.Json.string),
              ]),
            ),
          (),
        ),
      )
      |> Js.Promise.then_(result => {
           let _ = handleSignInResult(result);
           Js.Promise.resolve();
         });
    ();
  };
  let handleBack = () => {
    let _ = Next.Router.back();
    ();
  };
  let handleAuthenticateGoogle = () => {
    setIsMenuOpen(_ => false);
    setIsAuthenticating(_ => true);

    let _ =
      Webview.(
        postMessageForResult(WebEvent.make(~type_="AUTH_SIGN_IN_GOOGLE", ()))
      )
      |> Js.Promise.then_(result => {
           let _ = handleSignInResult(result);
           Js.Promise.resolve();
         });
    ();
  };
  let handleToggleIsMenuOpen = () => setIsMenuOpen(open_ => !open_);

  <main
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
    ])}>
    <Header
      className={Cn.fromList([
        "py-2",
        "px-4",
        "items-center",
        "bg-darkAccent",
        "border-b",
        "border-dotted",
        "border-lightDisabled",
        "w-full",
      ])}>
      <div className={Cn.fromList(["flex", "flex-row", "flex-1"])}>
        <MaterialUi.IconButton
          size=`Small
          edge=MaterialUi.IconButton.Edge.start
          onClick={_ => {handleBack()}}
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }
          classes={MaterialUi.IconButton.Classes.make(
            ~root=Cn.fromList(["p-0"]),
            (),
          )}>
          <Svg
            className={Cn.fromList([
              "w-5",
              "h-5",
              "pointer-events-none",
              "opacity-75",
            ])}
            icon=Svg.back
          />
        </MaterialUi.IconButton>
        <h1
          className={Cn.fromList([
            "ml-2",
            "font-sans",
            "text-lightPrimary",
            "text-lg",
            "font-bold",
          ])}>
          {React.string("Sign In")}
        </h1>
      </div>
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
                  Cn.fromList([
                    "font-sans",
                    "flex",
                    "flex-1",
                    "items-stretch",
                  ]),
                (),
              )}>
              <a
                href=Constants.resetPasswordUrl
                className={Cn.fromList(["flex", "flex-1", "items-center"])}>
                {React.string("Reset Password")}
              </a>
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
    </Header>
    <main
      className={Cn.fromList(["flex", "flex-col", "px-4", "py-4", "flex-1"])}>
      <MaterialUi.Collapse
        collapsedHeight={MaterialUi.Collapse.CollapsedHeight.int(0)}
        _in={Js.Option.isSome(authenticationError)}>
        <div
          className={Cn.fromList([
            "flex",
            "flex-row",
            "items-center",
            "border",
            "border-dotted",
            "border-lightSecondary",
            "bg-errorRed",
            "p-3",
            "mb-4",
          ])}>
          <Svg
            className={Cn.fromList(["w-5", "h-5", "pointer-events-none"])}
            icon=Svg.errorOutline
          />
          <p
            className={Cn.fromList([
              "font-sans",
              "text-lightPrimary",
              "leading-none",
              "text-sm",
              "ml-4",
            ])}>
            {React.string(
               switch (authenticationError) {
               | Some(UserNotFound) => "No user exists for this email. Verify it is correct, or sign up."
               | Some(UserNotAuthorized)
               | Some(AuthenticationFailed)
               | None => "Unable to sign in. Verify that your credentials are correct."
               },
             )}
          </p>
        </div>
      </MaterialUi.Collapse>
      <AuthenticationFields
        onChange={v => setAuthenticationValue(_ => v)}
        onSubmit=handleSubmit
        type_=AuthenticationFields.SignIn
      />
      <div
        className={Cn.fromList([
          "flex",
          "flex-row",
          "items-center",
          "justify-end",
          "mt-6",
        ])}>
        <MaterialUi.Button
          onClick={_ => handleSubmit()}
          fullWidth=false
          classes={MaterialUi.Button.Classes.make(
            ~label=Cn.fromList(["font-sans"]),
            (),
          )}
          style={
            isAuthenticating
              ? ReactDOM.Style.make(
                  ~paddingLeft="42px",
                  ~paddingRight="42px",
                  (),
                )
              : ReactDOM.Style.make()
          }
          variant=`Contained
          size=`Large
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }>
          {isAuthenticating
             ? <MaterialUi.CircularProgress
                 variant=`Indeterminate
                 size={MaterialUi.CircularProgress.Size.int(26)}
                 classes={MaterialUi.CircularProgress.Classes.make(
                   ~colorPrimary=Cn.fromList(["text-black"]),
                   (),
                 )}
               />
             : React.string("Sign In")}
        </MaterialUi.Button>
      </div>
    </main>
  </main>;
};

let page = "authenticate/sign-in.js";
