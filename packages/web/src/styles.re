let cn = Cn.make;

let style = ReactDOMRe.Style.make;

let rem = r => Js.Float.toString(r) ++ "rem";

let unsafeStyle = (style, unsafeList) =>
  List.fold_left(
    (acc, (prop, value)) =>
      ReactDOMRe.Style.unsafeAddProp(acc, prop, value),
    style,
    unsafeList,
  );
