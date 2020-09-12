open Styles;

[@react.component]
let default = () => {
  let authentication = Hooks_CurrentUserInfo.use();

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (authentication) {
          | Authenticated(currentUser) =>
            Routes.CreatorsIdAnnotations.path(
              ~creatorUsername=currentUser.username,
            )
            ->Next.Router.replace
          | _ => ()
          };
        None;
      },
      [|authentication|],
    );

  let handleAuthenticateGoogle = () => {
    let didPostMessage =
      Webview.(postMessage(WebEvent.make(~type_="AUTH_SIGN_IN")));

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
      onClick={_ => handleAuthenticateGoogle()}
      _TouchRippleProps={
        "classes": {
          "child": cn(["bg-white"]),
          "rippleVisible": cn(["opacity-75"]),
        },
      }
      size=`Large
      classes={MaterialUi.Button.Classes.make(
        ~root=cn(["py-4"]),
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
      {React.string("Sign In With Google")}
    </MaterialUi.Button>
  </div>;
};

let page = "authenticate.js";
