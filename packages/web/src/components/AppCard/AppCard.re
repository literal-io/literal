[@react.component]
let make =
    (
      ~labelClassName=?,
      ~nativeAppVersion=?,
      ~isFlavorFoss=false,
      ~webAppVersion,
      ~onClickChangelog,
      ~onClickPrivacyPolicy,
      ~onClickRateThisApp,
    ) => {
  let versionSeparator = isFlavorFoss ? "-foss-" : "-";
  let displayVersion =
    nativeAppVersion->Belt.Option.map(nativeAppVersion =>
      "(v" ++ nativeAppVersion ++ versionSeparator ++ webAppVersion ++ ")"
    );

  <>
    <h2
      className={Cn.fromList([
        "font-sans",
        "text-xs",
        "text-lightDisabled",
        "mb-2",
        Cn.take(labelClassName),
      ])}>
      {React.string("App Information")}
    </h2>
    <MaterialUi.Card
      variant=`Outlined
      classes={MaterialUi.Card.Classes.make(
        ~root=Cn.fromList(["bg-darkAccent", "border-lightDivider", "px-0"]),
        (),
      )}>
      <MaterialUi.CardActions
        classes={MaterialUi.CardActions.Classes.make(
          ~root=Cn.fromList(["flex", "flex-col", "p-0", "items-stretch"]),
          (),
        )}>
        {displayVersion
         ->Belt.Option.map(displayVersion =>
             <MaterialUi.Button
               fullWidth=true
               size=`Medium
               variant=`Text
               onClick={_ => onClickChangelog()}
               classes={MaterialUi.Button.Classes.make(
                 ~root=Cn.fromList(["justify-start", "py-2", "px-4"]),
                 ~label=
                   Cn.fromList([
                     "font-sans",
                     "normal-case",
                     "flex",
                     "justify-between",
                     "flex-row",
                   ]),
                 (),
               )}
               _TouchRippleProps={
                 "classes": {
                   "child": Cn.fromList(["bg-white"]),
                   "rippleVisible": Cn.fromList(["opacity-75"]),
                 },
               }>
               <span className={Cn.fromList(["text-lightSecondary"])}>
                 {React.string("CHANGELOG")}
               </span>
               <span
                 className={Cn.fromList(["text-lightDisabled", "text-xs"])}>
                 {React.string(displayVersion)}
               </span>
             </MaterialUi.Button>
           )
         ->Belt.Option.getWithDefault(
             <Skeleton
               variant=`text
               className={Cn.fromList(["h-6", "w-32", "my-2", "mx-4"])}
             />,
           )}
        <div
          className={Cn.fromList([
            "border-t",
            "border-b",
            "border-dotted",
            "border-lightDisabled",
            "ml-0",
          ])}>
          <MaterialUi.Button
            fullWidth=true
            size=`Medium
            variant=`Text
            onClick={_ => onClickPrivacyPolicy()}
            classes={MaterialUi.Button.Classes.make(
              ~root=Cn.fromList(["justify-start", "py-2", "px-4"]),
              ~label=Cn.fromList(["font-sans", "text-lightSecondary"]),
              (),
            )}
            _TouchRippleProps={
              "classes": {
                "child": Cn.fromList(["bg-white"]),
                "rippleVisible": Cn.fromList(["opacity-75"]),
              },
            }>
            {React.string("Privacy Policy")}
          </MaterialUi.Button>
        </div>
        <MaterialUi.Button
          fullWidth=true
          size=`Medium
          variant=`Text
          onClick={_ => onClickRateThisApp()}
          classes={MaterialUi.Button.Classes.make(
            ~root=Cn.fromList(["justify-start", "ml-0", "py-2", "px-4"]),
            ~label=Cn.fromList(["font-sans", "text-lightSecondary"]),
            (),
          )}
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-white"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }>
          {React.string("Rate This App")}
        </MaterialUi.Button>
      </MaterialUi.CardActions>
    </MaterialUi.Card>
  </>;
};
