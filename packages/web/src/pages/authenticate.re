open Styles;

[@react.component]
let default = () => {
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
