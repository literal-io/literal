[@react.component]
let make = (~icon, ~label, ~className=?, ~onClick=?, ~href=?) => {
  <MaterialUi.IconButton
    size=`Medium
    edge=MaterialUi.IconButton.Edge._false
    ?onClick
    ?href
    _TouchRippleProps={
      "classes": {
        "child": Cn.fromList(["bg-white"]),
        "rippleVisible": Cn.fromList(["opacity-75"]),
      },
    }
    classes={MaterialUi.IconButton.Classes.make(
      ~root=
        Cn.fromList([
          "p-10",
          "border-b",
          "border-lightSecondary",
          "border-dotted",
          "bg-darkAccent",
          "rounded-sm",
          Cn.take(className),
        ]),
      (),
    )}>
    <Svg
      className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
      icon
    />
    <div
      className={Cn.fromList([
        "absolute",
        "left-0",
        "right-0",
        "bottom-0",
        "mb-2",
        "flex",
        "items-center",
        "justify-center",
      ])}>
      <span
        className={Cn.fromList([
          "font-sans",
          "text-xs",
          "text-lightDisabled",
          "uppercase",
          "italic",
        ])}>
        {React.string(label)}
      </span>
    </div>
  </MaterialUi.IconButton>;
};
