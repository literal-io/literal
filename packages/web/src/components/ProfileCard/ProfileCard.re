module SignedInData = {
  [@react.component]
  let make = (~email, ~onClickSignOut, ~isMergingUserIdentities=false) =>
    <MaterialUi.Card
      variant=`Outlined
      classes={MaterialUi.Card.Classes.make(
        ~root=Cn.fromList(["bg-darkAccent", "border-lightDivider"]),
        (),
      )}>
      <MaterialUi.CardContent
        classes={MaterialUi.CardContent.Classes.make(
          ~root=Cn.fromList(["py-6"]),
          (),
        )}>
        <span
          className={Cn.fromList([
            "font-sans",
            "text-lightDisabled",
            "text-lg",
            "font-bold",
            "mr-2",
          ])}>
          {React.string("~")}
        </span>
        <span
          className={Cn.fromList([
            "font-sans",
            "text-lightPrimary",
            "text-lg",
            "font-bold",
          ])}>
          {React.string(email)}
        </span>
      </MaterialUi.CardContent>
      {isMergingUserIdentities
         ? <MaterialUi.CardActions
             classes={MaterialUi.CardActions.Classes.make(
               ~root=
                 Cn.fromList([
                   "flex",
                   "flex-row",
                   "border-t",
                   "border-dotted",
                   "border-lightDisabled",
                   "px-4",
                 ]),
               (),
             )}>
             <MaterialUi.CircularProgress
               variant=`Indeterminate
               size={MaterialUi.CircularProgress.Size.int(18)}
               classes={MaterialUi.CircularProgress.Classes.make(
                 ~colorPrimary=Cn.fromList(["text-white"]),
                 (),
               )}
             />
             <span
               className={Cn.fromList([
                 "font-sans",
                 "text-lightSecondary",
                 "text-sm",
                 "ml-4",
                 "py-2",
               ])}>
               {React.string("Migrating your data...")}
             </span>
           </MaterialUi.CardActions>
         : <MaterialUi.CardActions
             classes={MaterialUi.CardActions.Classes.make(
               ~root=
                 Cn.fromList([
                   "p-0",
                   "border-t",
                   "border-dotted",
                   "border-lightDisabled",
                 ]),
               (),
             )}>
             <MaterialUi.Button
               size=`Medium
               variant=`Text
               onClick={_ => onClickSignOut()}
               classes={MaterialUi.Button.Classes.make(
                 ~root={Cn.fromList(["justify-start", "py-2"])},
                 ~label=
                   {Cn.fromList(["font-sans", "text-lightSecondary", "px-2"])},
                 (),
               )}
               _TouchRippleProps={
                 "classes": {
                   "child": Cn.fromList(["bg-white"]),
                   "rippleVisible": Cn.fromList(["opacity-75"]),
                 },
               }
               fullWidth=true>
               {React.string("Sign Out")}
             </MaterialUi.Button>
           </MaterialUi.CardActions>}
    </MaterialUi.Card>;
};

module GuestUserData = {
  [@react.component]
  let make = () => {
    let Providers_ModalNavigation.{setOnNext} =
      React.useContext(Providers_ModalNavigation.context);

    let handleModalNavigation = _ => {
      setOnNext(Some(Next.Router.back));
    };

    <MaterialUi.Card
      variant=`Outlined
      classes={MaterialUi.Card.Classes.make(
        ~root=Cn.fromList(["bg-darkAccent", "border-lightDivider"]),
        (),
      )}>
      <MaterialUi.CardContent
        classes={MaterialUi.CardContent.Classes.make(
          ~root=Cn.fromList(["mb-4"]),
          (),
        )}>
        <span className={Cn.fromList(["font-sans", "text-lightPrimary"])}>
          {React.string("Create an account to complete your onboarding.")}
        </span>
      </MaterialUi.CardContent>
      <MaterialUi.CardActions>
        <Next.Link href={Routes.AuthenticateSignUp.path()} passHref=true>
          <MaterialUi.Button
            onClick=handleModalNavigation
            size=`Medium
            variant=`Contained
            classes={MaterialUi.Button.Classes.make(
              ~label=Cn.fromList(["font-sans"]),
              (),
            )}
            _TouchRippleProps={
              "classes": {
                "child": Cn.fromList(["bg-black"]),
                "rippleVisible": Cn.fromList(["opacity-75"]),
              },
            }>
            {React.string("Sign Up")}
          </MaterialUi.Button>
        </Next.Link>
        <Next.Link href={Routes.AuthenticateSignIn.path()} passHref=true>
          <MaterialUi.Button
            onClick=handleModalNavigation
            size=`Medium
            variant=`Text
            classes={MaterialUi.Button.Classes.make(
              ~root=Cn.fromList(["ml-4"]),
              ~label=Cn.fromList(["font-sans", "text-lightSecondary"]),
              (),
            )}
            _TouchRippleProps={
              "classes": {
                "child": Cn.fromList(["bg-white"]),
                "rippleVisible": Cn.fromList(["opacity-75"]),
              },
            }>
            {React.string("Sign In")}
          </MaterialUi.Button>
        </Next.Link>
      </MaterialUi.CardActions>
    </MaterialUi.Card>;
  };
};

[@react.component]
let make = (~user: Providers_Authentication_User.t, ~onClickSignOut) => {
  let main =
    switch (user) {
    | SignedInUser({attributes: {email}}) =>
      <SignedInData email onClickSignOut />
    | SignedInUserMergingIdentites({attributes: {email}}) =>
      <SignedInData email onClickSignOut isMergingUserIdentities=true />
    | GuestUser(_) => <GuestUserData />
    | Unknown
    | SignedOutPromptAuthentication => <Loading />
    };

  <>
    <h2
      className={Cn.fromList([
        "font-sans",
        "text-xs",
        "text-lightDisabled",
        "mb-2",
      ])}>
      {React.string("Profile")}
    </h2>
    main
  </>;
};
