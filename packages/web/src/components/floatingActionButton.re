open Styles;

[@react.component]
let make = (~onClick=?, ~className=?, ~children) =>
  <div className={cn(["h-14", "w-14", Cn.unpack(className)])} ?onClick>
    <div
      className={cn([
        "flex",
        "justify-center",
        "items-center",
        "bg-white",
        "h-14",
        "w-14",
        "rounded-full",
        "absolute",
        "top-0",
        "left-0",
      ])}>
      children
    </div>
    <div
      className={cn([
        "h-14",
        "w-14",
        "absolute",
        "top-0",
        "left-0",
        "mt-1/2",
        "ml-1/2",
        "border",
        "border-white",
        "rounded-full",
      ])}
    />
  </div>;
