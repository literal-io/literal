let styles = [%raw "require('./TagsList.module.css')"];

type tag = {
  id: option(string),
  href: option(string),
  text: string,
};

module TagTextField = {
  [@react.component]
  let make = (~tag, ~onChange) => {
    let keyEventHandled = React.useRef(false);
    let inputRef = React.useRef(Js.Nullable.null);

    let (value, setValue) = React.useState(() => tag.text);

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
          onChange(Some({href: None, text: value, id: None}));
        } else if (keyCode === 8  /*** backspace */
                   && Js.String2.length(value) === 0) {
          onChange(None);
        };
      };

    let handleBlur = _ => {
      Js.String2.length(value) === 0
        ? onChange(None)
        : onChange(Some({href: None, text: value, id: None}));
    };

    <MaterialUi.TextField
      ref={inputRef->ReactDOMRe.Ref.domRef}
      value={MaterialUi.TextField.Value.string(value)}
      onChange=handleChange
      autoFocus=true
      fullWidth=true
      multiline=true
      classes={MaterialUi.TextField.Classes.make(
        ~root=Cn.fromList(["mx-2", "mb-1"]),
        (),
      )}
      _InputProps={
        "classes":
          MaterialUi.Input.Classes.make(
            ~input=
              Cn.(
                fromList([
                  "font-sans",
                  "text-lightPrimary",
                  "font-medium",
                  "text-lg",
                ])
              ),
            ~underline=Cn.(fromList([styles##underline])),
            (),
          ),
      }
      inputProps={
        "onChange": handleChange,
        "onKeyUp": handleKeyUp,
        "onKeyDown": handleKeyDown,
        "onBlur": handleBlur,
      }
    />;
  };
};

module TagButton = {
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

    let tag =
      <MaterialUi.Button
        ref={buttonRef->ReactDOMRe.Ref.domRef}
        variant=`Text
        fullWidth=true
        ?disabled
        onTouchStart=handleTouchStart
        onTouchEnd=handleTouchEnd
        onClick={_ => {
          let _ =
            Service_Analytics.(
              track(Click({action: "tag", label: Some(text)}))
            );
          ();
        }}
        classes={MaterialUi.Button.Classes.make(
          ~root=Cn.fromList(["mb-1"]),
          ~text=
            Cn.fromList([
              "font-sans",
              "text-lightPrimary",
              "font-medium",
              "text-lg",
              "normal-case",
              "justify-start",
            ]),
          (),
        )}>
        <span
          className={Cn.fromList([
            "border-b",
            "border-white",
            "border-opacity-50",
          ])}>
          {React.string(text)}
        </span>
      </MaterialUi.Button>;
    switch (href) {
    | Some(href) =>
      <Next.Link
        key=href
        _as=href
        href=Routes.CreatorsIdAnnotationCollectionsId.staticPath
        passHref=true>
        tag
      </Next.Link>
    | None => tag
    };
  };
};

[@react.component]
let make = (~value, ~disabled=?) => {
  let (editingId, setEditingId) = React.useState(_ => None);

  let handleChange = (~tag, ~idx) => {
    let _ = setEditingId(_ => None);
    ();
  };

  let tags =
    value
    ->Belt.Array.keep(({text}) => text != "recent")
    ->Belt.Array.mapWithIndex((idx, tag) =>
        tag.id == editingId
          ? <TagTextField
              key={tag.text}
              onChange={tag => handleChange(~tag, ~idx)}
              tag
            />
          : <TagButton
              key={tag.text}
              text={tag.text}
              href={tag.href}
              ?disabled
              onLongPress={() => setEditingId(_ => tag.id)}
            />
      )
    ->React.array;

  <div className={Cn.fromList(["flex", "flex-col", "pt-8", "pb-4"])}>
    tags
  </div>;
};
