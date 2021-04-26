open Styles;

[@react.component]
let make = (~className=?, ~lineCount=8) => {
  let (margins, _setMargins) =
    React.useState(_ => {
      let margins = [|"", "mr-1", "mr-2", "mr-3", "mr-4", "mr-5"|];
      Belt.Array.make(lineCount, 0)
      ->Belt.Array.map(_ =>
          margins[Js.Math.random_int(0, Js.Array2.length(margins))]
        );
    });

  <div
    className={Cn.fromList(["mx-4"])}
    style={ReactDOM.Style.make(~paddingTop="4.5rem", ())}>
    <div className={Cn.fromList(["p-4", "bg-darkAccent", "rounded-t-sm"])}>
      {Belt.Array.make(lineCount, 0)
       ->Belt.Array.mapWithIndex((idx, _) => {
           <Skeleton
             key={string_of_int(idx)}
             variant=`text
             className={Cn.fromList(["mb-1", "h-6", margins[idx]])}
           />
         })
       ->React.array}
    </div>
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "border-dotted",
        "border-t",
        "border-lightDisabled",
        "p-4",
        "bg-darkAccent",
        "rounded-b-sm"
      ])}>
      <Skeleton
        variant=`text
        className={Cn.fromList(["h-6", "w-40"])}
      />
    </div>
  </div>;
};
