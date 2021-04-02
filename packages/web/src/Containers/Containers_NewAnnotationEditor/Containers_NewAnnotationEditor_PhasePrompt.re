module PromptIconButton = {
  [@react.component]
  let make = (~icon, ~label, ~className=?, ~onClick) => {
    <MaterialUi.IconButton
      size=`Medium
      edge=MaterialUi.IconButton.Edge._false
      onClick
      _TouchRippleProps={
        "classes": {
          "child": Cn.fromList(["bg-white"]),
          "rippleVisible": Cn.fromList(["opacity-75"]),
        },
      }
      classes={MaterialUi.IconButton.Classes.make(
        ~root=
          Cn.fromList([
            "p-10",
            "border-b",
            "border-lightSecondary",
            "border-dotted",
            "bg-darkAccent",
            "rounded-sm",
            Cn.take(className),
          ]),
        (),
      )}>
      <Svg
        placeholderViewBox="0 0 24 24"
        className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
        icon
      />
      <div
        className={Cn.fromList([
          "absolute",
          "left-0",
          "right-0",
          "bottom-0",
          "mb-2",
          "flex",
          "items-center",
          "justify-center",
        ])}>
        <span
          className={Cn.fromList([
            "font-sans",
            "text-xs",
            "text-lightDisabled",
            "uppercase",
            "italic",
          ])}>
          {React.string(label)}
        </span>
      </div>
    </MaterialUi.IconButton>;
  };
};

[@react.component]
let make = (~onCreateFromText, ~onCreateFromFile, ~onCreateFromWeb) => {
  let inputRef = React.useRef(Js.Nullable.null);
  let handleFileInputChange = ev => {
    let _ = ev->ReactEvent.Form.persist;
    let files = ReactEvent.Form.target(ev)##files;
    let _ =
      Js.Array2.length(files) > 0
        ? files->Belt.Array.getExn(0)->onCreateFromFile
        : Sentry.captureMessage("Expected files, but got none.");
    ();
  };

  let handleWebButtonClick = _ => {
    let _ =
      Service_Analytics.(
        track(Click({action: "create from web", label: None}))
      );
    onCreateFromWeb();
  };

  let handleTextButtonClick = _ => {
    let _ =
      Service_Analytics.(
        track(Click({action: "create from text", label: None}))
      );
    onCreateFromText();
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
    className={Cn.fromList(["flex", "flex-col", "justify-between", "flex-1"])}>
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "justify-center",
        "items-center",
        "flex-auto",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "flex-col",
          "justify-center",
          "items-center",
          "pb-4",
          "mx-6",
        ])}>
        <PromptIconButton
          label="web"
          icon=Svg.language
          onClick=handleWebButtonClick
          className={Cn.fromList(["mb-8"])}
        />
        <PromptIconButton
          label="text"
          icon=Svg.textFields
          onClick=handleTextButtonClick
          className={Cn.fromList(["mb-8"])}
        />
        <PromptIconButton
          label="screenshot"
          icon=Svg.textSnippet
          onClick=handleFileButtonClick
        />
        <input
          type_="file"
          accept="image/*"
          ref={inputRef->ReactDOM.Ref.domRef}
          onChange=handleFileInputChange
          className={Cn.fromList(["absolute", "opacity-0", "-z-10"])}
        />
      </div>
    </div>
    <BottomAlert
      text="Use the sharesheet to import highlighted text, screenshots, and web pages directly from the apps you use to read."
    />
  </div>;
};
