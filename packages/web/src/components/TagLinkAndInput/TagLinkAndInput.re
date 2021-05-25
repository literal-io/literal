module TextField = {
  [@react.component]
  let make = (~text, ~onChange, ~onBlur, ~onFocus=?) => {
    let keyEventHandled = React.useRef(false);

    let (value, setValue) = React.useState(() => text);

    let handleChange = ev =>
      switch (ReactEvent.Form.nativeEvent(ev)##inputType) {
      /*** handled in handleKeyUp **/
      | "insertLineBreak" => ()
      | _ =>
        let newValue = ev->ReactEvent.Form.target->(el => el##value);

        /** handled in handleKeyUp: subsequent delete when tag input is empty **/
        let emptyAndNoChange =
          Js.String2.length(newValue) === 0 && Js.String2.length(value) === 0;

        /**
         * Android does not produce usable keyCodes, so we have to manually diff input values
         * to determine when a newline is inserted.
         *
         * https://bugs.chromium.org/p/chromium/issues/detail?id=118639
         */
        let androidAndEndsWithNewLine =
          switch (Constants.bowser()->Bowser.getOS->Bowser.getOSName) {
          | Some(`Android) when Js.String2.endsWith(newValue, "\n") => true
          | _ => false
          };

        if (!emptyAndNoChange && !androidAndEndsWithNewLine) {
          keyEventHandled.current = true;
          setValue(_ => newValue);
        } else if (androidAndEndsWithNewLine) {
          onChange(Some(value));
          onBlur();
        };
      };

    let handleKeyDown = ev => {
      keyEventHandled.current = ReactEvent.Keyboard.isDefaultPrevented(ev);
    };

    let handleKeyUp = ev =>
      if (!keyEventHandled.current) {
        /** on Android, keyCode seems to only be correct for backspace. **/
        let keyCode = ReactEvent.Keyboard.keyCode(ev);
        let newValue = ev->ReactEvent.Keyboard.target->(el => el##value);

        if (keyCode === 13  /*** enter **/
            && Js.String2.length(newValue) > 0) {
          onChange(Some(value));
          onBlur();
        } else if (keyCode === 8  /*** backspace **/
                   && Js.String2.length(newValue) === 0) {
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
          "text-base",
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
        "onFocus": onFocus,
        "enterKeyHint": "done",
      },
    );
  };
};

module Link = {
  [@react.component]
  let make = (~href, ~text, ~disabled=?, ~onLongPress) => {
    let timeoutId = React.useRef(None);
    let router = Next.Router.useRouter();

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
      let annotationCollectionIdComponent =
        router.Next.query
        ->Js.Json.decodeObject
        ->Belt.Option.flatMap(o =>
            Js.Dict.get(o, "annotationCollectionIdComponent")
          )
        ->Belt.Option.flatMap(Js.Json.decodeString);

      let shouldNavigate =
        !
          Belt.Option.eq(
            href->Belt.Option.map(
              Lib_GraphQL.AnnotationCollection.idComponent,
            ),
            annotationCollectionIdComponent,
            (a, b) =>
            a === b
          );

      let _ =
        if (shouldNavigate) {
          href->Belt.Option.forEach(href =>
            Next.Router.pushWithAs(
              Routes.CreatorsIdAnnotationCollectionsId.staticPath,
              href,
            )
          );
        };

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
        "text-base",
        "normal-case",
        "justify-start",
      ])}>
      {React.string(text)}
    </span>;
  };
};

[@react.component]
let make = (~href, ~text, ~disabled=?, ~onChange, ~onFocus=?, ~onBlur=?) => {
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

  let handleBlur = _ => {
    let _ = setIsEditing(_ => false);
    onBlur->Belt.Option.forEach(cb => cb());
    ();
  };
  let handleLongPress = _ => {
    setIsEditing(_ => true);
    let _ = onFocus->Belt.Option.forEach(cb => cb());
    ()
  };

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
       ? <TextField text onChange onBlur=handleBlur ?onFocus />
       : <Link text href ?disabled onLongPress=handleLongPress />}
  </div>;
};
