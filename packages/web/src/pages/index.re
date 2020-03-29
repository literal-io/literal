open Styles;

[@react.component]
let default = () =>
  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "px-6",
      "flex",
      "flex-col",
    ])}>
    <Header title="Create" />
    <div className={cn(["my-4"])}> <TextEditor /> </div>
  </div>;
