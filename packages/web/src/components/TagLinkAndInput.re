module TextField = {
  [@react.component]
  let make = (~text, ~onChange, ~onBlur) => {
    let keyEventHandled = React.useRef(false);

    let (value, setValue) = React.useState(() => text);

    let handleChange = ev => {
      let data =
        ev
        ->ReactEvent.Form.nativeEvent
        ->(ev => ev##data)
        ->Js.Nullable.toOption;
      let inputType = ev->ReactEvent.Form.nativeEvent->(ev => ev##inputType);
      let os = Constants.bowser()->Bowser.getOS->Bowser.getOSName;

      let newValue =
        switch (inputType, data, os) {
        | ("insertText", Some(insertedText), _) =>
          Some(value ++ insertedText)
        | ("insertCompositionText", Some(insertedText), Some(`Android)) =>
          Some(insertedText)
        | ("deleteContentBackward", _, _) when Js.String.length(value) > 0 =>
          let newValue =
            Js.String2.slice(
              value,
              ~from=0,
              ~to_=Js.String2.length(value) - 1,
            );
          let _ =
            if (Js.String.length(newValue) === 0) {
              ev->ReactEvent.Form.stopPropagation;
            };
          Some(newValue);
        | _ => None
        };

      let _ =
        switch (newValue) {
        | Some(newValue) =>
          keyEventHandled.current = true;
          setValue(_ => newValue);
        | None => ()
        };
      ();
    };

    let handleKeyDown = ev => {
      keyEventHandled.current = ReactEvent.Keyboard.isDefaultPrevented(ev);
    };

    let handleKeyUp = ev =>
      if (!keyEventHandled.current) {
        let keyCode = ReactEvent.Keyboard.keyCode(ev);
        if (keyCode === 13 /*** enter **/ && Js.String2.length(value) > 0) {
          onChange(Some(value));
          onBlur();
        } else if (keyCode === 8  /*** backspace **/
                   && Js.String2.length(value) === 0) {
          onChange(None);
          onBlur();
        };
      };

    let handleBlur = _ => {
      Js.String2.length(value) === 0
        ? onChange(None) : onChange(Some(value));
      let _ = onBlur();
      ();
    };

    React.cloneElement(
      <MaterialUi.TextareaAutosize
        onChange=handleChange
        value={MaterialUi.TextareaAutosize.Value.string(value)}
        className={Cn.fromList([
          "font-sans",
          "text-lightPrimary",
          "font-medium",
          "text-lg",
          "bg-black",
          "outline-none",
          "resize-none",
        ])}
      />,
      {
        "autoFocus": true,
        "onKeyUp": handleKeyUp,
        "onKeyDown": handleKeyDown,
        "onBlur": handleBlur,
      },
    );
  };
};

module Link = {
  [@react.component]
  let make = (~href, ~text, ~disabled=?, ~onLongPress) => {
    let timeoutId = React.useRef(None);

    let _ =
      React.useEffect0(() => {
        Some(
          () => {
            timeoutId.current->Belt.Option.forEach(Js.Global.clearTimeout);
            timeoutId.current = None;
          },
        )
      });

    let handleTouchStart = ev => {
      let _ = ev->ReactEvent.Touch.persist;
      timeoutId.current =
        Some(
          Js.Global.setTimeout(
            () => {
              onLongPress();
              ();
            },
            400,
          ),
        );
    };

    let handleTouchEnd = _ => {
      timeoutId.current->Belt.Option.forEach(Js.Global.clearTimeout);
      timeoutId.current = None;
    };

    let handleClick = _ => {
      let _ =
        href->Belt.Option.forEach(href =>
          Next.Router.pushWithAs(
            Routes.CreatorsIdAnnotationCollectionsId.staticPath,
            href,
          )
        );
      let _ =
        Service_Analytics.(
          track(Click({action: "tag", label: Some(text)}))
        );
      ();
    };

    <span
      onTouchStart=handleTouchStart
      onTouchEnd=handleTouchEnd
      onClick=handleClick
      className={Cn.fromList([
        "font-sans",
        "text-lightSecondary",
        "font-medium",
        "text-lg",
        "normal-case",
        "justify-start",
      ])}>
      {React.string(text)}
    </span>;
  };
};

[@react.component]
let make = (~href, ~text, ~disabled=?, ~onChange) => {
  let (isEditing, setIsEditing) = React.useState(_ => false);
  let elemRef = React.useRef(Js.Nullable.null);
  let _ =
    React.useEffect0(() => {
      let disableContextMenu = ev => {
        let _ = Webapi.Dom.Event.preventDefault(ev);
        ();
      };

      let _ =
        elemRef.current
        ->Js.Nullable.toOption
        ->Belt.Option.forEach(el => {
            let _ =
              Webapi.Dom.HtmlElement.addEventListener(
                "contextmenu",
                disableContextMenu,
                el->Webapi.Dom.HtmlElement.ofElement->Belt.Option.getExn,
              );
            ();
          });
      None;
    });

  <div
    ref={elemRef->ReactDOMRe.Ref.domRef}
    className={Cn.fromList([
      isEditing
        ? Cn.fromList([
            "flex",
            "items-stretch",
            "flex-col",
            "border-lightPrimary",
          ])
        : Cn.fromList(["inline-flex", "flex-col", "border-lightDisabled"]),
      "border-b",
      "border-dotted",
      "transition-all",
      "duration-300",
    ])}>
    {isEditing
       ? <TextField text onChange onBlur={_ => setIsEditing(_ => false)} />
       : <Link
           text
           href
           ?disabled
           onLongPress={_ => setIsEditing(_ => true)}
         />}
  </div>;
};
