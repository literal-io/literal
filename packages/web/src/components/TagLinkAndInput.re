module TextField = {
  [@react.component]
  let make = (~text, ~onChange, ~onBlur) => {
    let keyEventHandled = React.useRef(false);
    let inputRef = React.useRef(Js.Nullable.null);

    let (value, setValue) = React.useState(() => text);

    let _ =
      React.useEffect0(() => {
        let disableContextMenu = ev => {
          let _ = Webapi.Dom.Event.preventDefault(ev);
          ();
        };

        let _ =
          inputRef.current
          ->Js.Nullable.toOption
          ->Belt.Option.forEach(el => {
              let htmlElement =
                el->Webapi.Dom.HtmlElement.ofElement->Belt.Option.getExn;

              let _ =
                htmlElement
                |> Webapi.Dom.HtmlElement.addEventListener(
                     "contextmenu",
                     disableContextMenu,
                   );
              ();
            });

        Some(
          () => {
            inputRef.current
            ->Js.Nullable.toOption
            ->Belt.Option.forEach(el => {
                Webapi.Dom.HtmlElement.removeEventListener(
                  "contextmenu",
                  disableContextMenu,
                  el->Webapi.Dom.HtmlElement.ofElement->Belt.Option.getExn,
                )
              })
          },
        );
      });

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
          Some(value);
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
        } else if (keyCode === 8  /*** backspace **/
                   && Js.String2.length(value) === 0) {
          onChange(None);
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
        ref={inputRef->ReactDOMRe.Ref.domRef}
        onChange=handleChange
        value={MaterialUi.TextareaAutosize.Value.string(value)}
        className={Cn.fromList([
          "mb-1",
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
    let buttonRef = React.useRef(Js.Nullable.null);
    let timeoutId = React.useRef(None);

    let _ =
      React.useEffect0(() => {
        let disableContextMenu = ev => {
          let _ = Webapi.Dom.Event.preventDefault(ev);
          ();
        };

        let _ =
          buttonRef.current
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

        Some(
          () => {
            timeoutId.current->Belt.Option.forEach(Js.Global.clearTimeout);
            timeoutId.current = None;

            buttonRef.current
            ->Js.Nullable.toOption
            ->Belt.Option.forEach(el => {
                Webapi.Dom.HtmlElement.removeEventListener(
                  "contextmenu",
                  disableContextMenu,
                  el->Webapi.Dom.HtmlElement.ofElement->Belt.Option.getExn,
                )
              });
          },
        );
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
            500,
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
      ref={buttonRef->ReactDOMRe.Ref.domRef}
      onTouchStart=handleTouchStart
      onTouchEnd=handleTouchEnd
      onClick=handleClick
      className={Cn.fromList([
        "mb-1",
        "border-b",
        "border-white",
        "border-opacity-50",
        "font-sans",
        "text-lightPrimary",
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

  isEditing
    ? <TextField text onChange onBlur={_ => setIsEditing(_ => false)} />
    : <Link text href ?disabled onLongPress={_ => setIsEditing(_ => true)} />;
};
