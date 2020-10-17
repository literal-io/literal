[@react.component]
let make = (~onCreateFromText, ~onCreateFromFile) => {
  let inputRef = React.useRef(Js.Nullable.null);
  let handleFileInputChange = ev => {
    let files = ReactEvent.Form.target(ev)##files;
    let _ =
      Js.Array2.length(files) > 0
        ? files->Belt.Array.getExn(0)->onCreateFromFile
        : Sentry.captureMessage("Expected files, but got none.");
    ();
  };

  let handleFileButtonClick = _ => {
    let _ =
      Service_Analytics.(
        track(Click({action: "create from file", label: None}))
      );
    let _ =
      inputRef.current
      ->Js.Nullable.toOption
      ->Belt.Option.forEach(elem => {
          let _ =
            elem
            ->Webapi.Dom.Element.unsafeAsHtmlElement
            ->Webapi.Dom.HtmlElement.click;
          ();
        });
    ();
  };

  <div
    className={Cn.fromList([
      "flex",
      "flex-row",
      "justify-center",
      "items-start",
      "flex-auto",
      "pt-20",
    ])}>
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "flex-auto",
        "justify-center",
        "items-center",
        "pb-4",
        "mx-6",
        "border-b",
        "border-white-o50",
      ])}>
      <MaterialUi.IconButton
        size=`Medium
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => {
          let _ =
            Service_Analytics.(
              track(Click({action: "create from text", label: None}))
            );
          onCreateFromText();
        }}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["mr-20", "p-8"]),
          (),
        )}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
          icon=Svg.textFields
        />
      </MaterialUi.IconButton>
      <MaterialUi.IconButton
        size=`Medium
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => handleFileButtonClick()}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["p-8"]),
          (),
        )}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
          icon=Svg.textSnippet
        />
      </MaterialUi.IconButton>
      <input
        type_="file"
        accept="image/*"
        ref={inputRef->ReactDOM.Ref.domRef}
        onChange=handleFileInputChange
        className={Cn.fromList(["absolute", "opacity-0", "-z-10"])}
      />
    </div>
  </div>;
};
