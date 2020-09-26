let styles = [%raw "require('./TextInput_Loading.module.css')"];

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

  <div ?className>
    {Belt.Array.make(lineCount, 0)
     ->Belt.Array.mapWithIndex((idx, _) => {
         <MaterialUiLab.Skeleton
           key={string_of_int(idx)}
           variant=`text
           classes={
             "root": cn([styles##skeleton, "mb-1", "h-8", margins[idx]]),
           }
         />
       })
     ->React.array}
  </div>;
};
