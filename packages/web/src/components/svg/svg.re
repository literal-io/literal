open Styles;

let done_: string = [%raw "require('./done.svg')"];
let close: string = [%raw "require('./close.svg')"];
let back: string = [%raw "require('./back.svg')"];
let removeCircle: string = [%raw "require('./remove-circle.svg')"];
let delete: string = [%raw "require('./delete.svg')"];
let logo: string = [%raw "require('./logo.svg')"];
let arrowRight: string = [%raw "require('./arrow-right.svg')"];
let add: string = [%raw "require('./add.svg')"];
let textFields: string = [%raw "require('./text-fields.svg')"];
let textSnippet: string = [%raw "require('./text-snippet.svg')"];

type state = {
  isLoading: bool,
  src: string,
};

[@react.component]
let make =
    (
      ~className=?,
      ~style=?,
      ~placeholderClassName=?,
      ~onClick=?,
      ~placeholderViewBox,
      ~icon,
    ) => {
  let ({isLoading, src}, setState) =
    React.useState(() => {isLoading: true, src: icon});

  let _ =
    React.useEffect1(
      () => {
        if (src !== icon) {
          let _ = setState(_ => {isLoading: true, src: icon});
          ();
        };
        None;
      },
      [|icon|],
    );

  let renderPlaceholder = () =>
    <svg
      className={
        placeholderClassName->Belt.Option.getWithDefault(
          cn(["w-full", "h-full"]),
        )
      }
      viewBox=placeholderViewBox
      fill="none"
      xmlns="http://www.w3.org/2000/svg"
    />;

  let handleObjectElem = el => {
    let rawHandler = [%raw
      {|
      function(el, cb) {
        if (
          el &&
          el.contentWindow &&
          el.contentWindow.document &&
          el.contentWindow.document.readyState === "complete"
        ) {
          cb()
        }
      }
    |}
    ];
    let _ =
      if (isLoading) {
        rawHandler(el, () => setState(_ => {src, isLoading: false}));
      };
    ();
  };

  let base =
    <>
      {isLoading ? renderPlaceholder() : React.null}
      <object
        onLoad={_ => setState(_ => {isLoading: false, src})}
        type_="image/svg+xml"
        className={
          isLoading
            ? cn(["invisible", "absolute", "w-0", "h-0", "relative"])
            : cn([className->Cn.unpack])
        }
        style=?{isLoading ? None : style}
        data=src
        ref={ReactDOMRe.Ref.callbackDomRef(handleObjectElem)}
      />
    </>;
  switch (onClick) {
  | Some(onClick) =>
    <div className={cn([className->Cn.unpack, "relative"])}>
      base
      <div
        className={cn(["absolute", "inset-x-0", "inset-y-0"])}
        onClick={_ => onClick()}
      />
    </div>
  | None =>
    <div className={cn([className->Cn.unpack, "relative"])}> base </div>
  };
};
