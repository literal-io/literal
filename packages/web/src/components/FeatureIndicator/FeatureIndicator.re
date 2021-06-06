[@react.component]
let make = (~enabled, ~className=?) =>
  <div
    className={Cn.fromList([
      "mr-4",
      "w-4",
      "h-4",
      "flex-shrink-0",
      enabled
        ? Cn.fromList(["bg-lightPrimary"])
        : Cn.fromList(["border-lightDisabled", "border", "border-dotted"]),
      Cn.take(className),
    ])}
  />;
