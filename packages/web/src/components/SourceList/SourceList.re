[@react.component]
let make = (~data, ~renderItem, ~itemKey, ~itemClassName, ~className=?) =>
  <ul
    className={Cn.fromList([
      "w-full",
      "max-h-full",
      "overflow-y-auto",
      Cn.take(className),
    ])}>
    {data
     ->Belt.Array.mapWithIndex((idx, item) =>
         <li
           key={itemKey(~item, ~idx)} className={itemClassName(~item, ~idx)}>
           {renderItem(~item, ~idx)}
         </li>
       )
     ->React.array}
  </ul>;
