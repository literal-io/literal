open Styles;

let styles = [%raw "require('./TextInput_Tags.module.css')"];

[@react.component]
let make =
    (
      ~value,
      ~onValueChange,
      ~onKeyDown=?,
      ~className=?,
      ~disabled=?,
      ~onValueCommit,
    ) => {
  let inputRef = React.useRef(Js.Nullable.null);
  let keyEventHandled = React.useRef(false);

  let handleChange = ev => {
    let _ = ev->ReactEvent.Form.persist;

    let data =
      ev->ReactEvent.Form.nativeEvent->(ev => ev##data)->Js.Nullable.toOption;
    let inputType = ev->ReactEvent.Form.nativeEvent->(ev => ev##inputType);
    let os = Constants.bowser()->Bowser.getOS->Bowser.getOSName;

    let newValue =
      switch (inputType, data, os) {
      | ("insertText", Some(insertedText), _) => Some(value ++ insertedText)
      | ("insertCompositionText", Some(insertedText), Some(`Android)) =>
        Some(insertedText)
      | ("deleteContentBackward", _, _) when Js.String.length(value) > 0 =>
        let newPartial =
          Js.String2.slice(
            value,
            ~from=0,
            ~to_=Js.String2.length(value) - 1,
          );
        let _ =
          if (Js.String.length(newPartial) === 0) {
            ev->ReactEvent.Form.stopPropagation;
          };
        Some(newPartial);
      | _ => None
      };

    let _ =
      switch (newValue) {
      | Some(newValue) =>
        keyEventHandled.current = true;
        onValueChange(newValue);
      | None => ()
      };
    ();
  };

  let handleKeyUp = ev => {
    let _ =
      if (!keyEventHandled.current) {
        let keyCode = ReactEvent.Keyboard.keyCode(ev);
        if (keyCode == 13 && Js.String.length(value) > 0) {
          let _ = onValueCommit(value);
          ();
        };
      };
    ();
  };

  let handleKeyDown = ev => {
    let _ =
      switch (onKeyDown) {
      | Some(onKeyDown) => onKeyDown(ev)
      | None => ()
      };

    keyEventHandled.current = ReactEvent.Keyboard.isDefaultPrevented(ev);
    ();
  };

  let handleBlur = ev => {
    let _ = ReactEvent.Focus.persist(ev);
    if (Js.String.length(value) > 0) {
      onValueChange("");
    };
  };

  <div className=Cn.(fromList(["flex", "flex-col", take(className)]))>
    <div className={Cn.fromList(["mx-2", "mt-2"])}>
      <MaterialUi.TextField
        ?disabled
        value={MaterialUi.TextField.Value.string(value)}
        onChange=handleChange
        fullWidth=true
        multiline=true
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
          "ref": inputRef->ReactDOMRe.Ref.domRef,
        }
      />
    </div>
  </div>;
};
