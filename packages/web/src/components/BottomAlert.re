[@react.component]
let make = (~text) =>
  <div
    className={Cn.fromList([
      "border-t",
      "border-dotted",
      "border-lightDisabled",
      "flex",
      "flex-row",
      "items-center",
      "p-6",
    ])}>
    <p className={Cn.fromList(["text-lightSecondary", "mr-6", "text-xs"])}>
      {React.string(text)}
    </p>
    <Svg
      icon=Svg.helpOutline
      placeholderViewBox="0 0 24 24"
      className={Cn.fromList([
        "pointer-events-none",
        "w-6",
        "w-6",
        "opacity-75",
      ])}
    />
  </div>;
