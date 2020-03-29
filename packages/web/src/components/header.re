open Styles;

[@react.component]
let make = (~title) =>
  <>
    <div className={cn(["flex", "flex-row", "py-3"])}>
      <h1 className={cn(["text-white", "uppercase", "italic", "font-bold"])}>
        {React.string(title)}
      </h1>
    </div>
    <div
      className={cn(["h-2", "border-white", "border-t", "border-b"])}
    />
  </>;
