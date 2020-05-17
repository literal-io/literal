open Styles;

[@react.component]
let default = () => {
  let authentication = CurrentUserInfo.use();

  let _ =
    React.useEffect0(() => {
      let handleEvent = (ev: AwsAmplify.Hub.event(AwsAmplify.Hub.auth)) => {
        switch (ev.payload.event) {
        | "signIn" => Next.Router.replace("/notes")
        | _ => ()
        };
      };

      let _ = AwsAmplify.Hub.(listen(inst, `auth(handleEvent)));
      Some(
        () => {
          let _ = AwsAmplify.Hub.(remove(inst, `auth(handleEvent)));
          ();
        },
      );
    });

  let _ =
    React.useEffect1(
      () => {
        let _ =
          switch (authentication) {
          | Authenticated(_) => Next.Router.replace("/notes")
          | _ => ()
          };
        None;
      },
      [|authentication|],
    );

  let handleAuthenticateGoogle = () =>
    AwsAmplify.Auth.(
      federatedSignInWithOptions(
        inst,
        {provider: "Google", customState: None},
      )
    );

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
      classes=[
        Root(cn(["py-4"])),
        Label(
          cn([
            "text-white",
            "font-sans",
            "font-bold",
            "text-lg",
            "leading-none",
            "italic",
          ]),
        ),
        Outlined(cn(["border-white"])),
      ]
      variant=`Outlined>
      {React.string("Sign In With Google")}
    </MaterialUi.Button>
  </div>;
};
