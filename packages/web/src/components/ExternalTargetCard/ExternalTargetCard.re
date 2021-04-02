[@react.component]
let make =
    (
      ~externalTargetFragment as externalTarget,
      ~textualTargetFragment as textualTarget,
      ~onClick,
    ) => {
  let host =
    externalTarget##externalTargetId->Webapi.Url.make->Webapi.Url.host;

  <MaterialUi.Button
    onClick
    _TouchRippleProps={
      "classes": {
        "child": Cn.fromList(["bg-white"]),
        "rippleVisible": Cn.fromList(["opacity-75"]),
      },
    }
    classes={MaterialUi.Button.Classes.make(
      ~root=Cn.fromList(["p-0", "bg-darkAccent", "rounded-sm"]),
      ~label=Cn.fromList(["flex", "flex-col", "items-stretch"]),
      (),
    )}
    fullWidth=true
    variant=`Text>
    <TextInput.Annotation
      onChange={_ => ()}
      disabled=true
      value=textualTarget##value
      inputClasses={MaterialUi.Input.Classes.make(
        ~root=Cn.fromList(["p-4"]),
        ~inputMultiline=Cn.fromList(["px-0"]),
        (),
      )}
    />
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "border-dotted",
        "border-t",
        "border-lightDisabled",
        "p-4",
      ])}>
      <span
        className={Cn.fromList([
          "text-lightSecondary",
          "font-sans",
          "italic",
          "lowercase",
        ])}>
        {React.string(host)}
      </span>
    </div>
  </MaterialUi.Button>;
};
