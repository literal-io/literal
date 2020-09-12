open Styles;

let styles = [%raw "require('./TextInput_Tags.module.css')"];

module Value = {
  type t = {
    commits: array(string),
    partial: string,
  };

  let empty = () => {commits: [||], partial: ""};
};

[@react.component]
let make =
  React.forwardRef((~value, ~onChange, ~onKeyDown=?, ~className=?, ref_) => {
    let keyEventHandled = React.useRef(false);

    let handleChange = ev => {
      let _ = ev->ReactEvent.Form.persist;

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
          Some(Value.{...value, partial: value.partial ++ insertedText})
        | ("insertCompositionText", Some(insertedText), Some(`Android)) =>
          Some(Value.{...value, partial: insertedText})
        | ("deleteContentBackward", _, _)
            when Js.String.length(value.partial) > 0 =>
          let newPartial =
            Js.String2.slice(
              value.partial,
              ~from=0,
              ~to_=Js.String2.length(value.partial) - 1,
            );
          let _ =
            if (Js.String.length(newPartial) === 0) {
              ev->ReactEvent.Form.stopPropagation;
            };
          Some({...value, partial: newPartial});
        | _ => None
        };

      let _ =
        switch (newValue) {
        | Some(newValue) =>
          let _ = keyEventHandled->React.Ref.setCurrent(true);
          onChange(newValue);
        | None => ()
        };
      ();
    };

    let handleKeyUp = ev => {
      if (!React.Ref.current(keyEventHandled)) {
        let newValue =
          switch (ReactEvent.Keyboard.keyCode(ev)) {
          | 13 /*** enter **/ when Js.String.length(value.partial) > 0 =>
            Some(
              Value.{
                commits: Js.Array2.concat(value.commits, [|value.partial|]),
                partial: "",
              },
            )
          | 8
              /*** backspace **/
              when
                Js.String.length(value.partial) === 0
                && Js.Array2.length(value.commits) > 0 =>
            Some({
              ...value,
              commits:
                Belt.Array.slice(
                  value.commits,
                  ~offset=0,
                  ~len=Js.Array2.length(value.commits) - 1,
                ),
            })
          | _ => None
          };
        let _ =
          switch (newValue) {
          | Some(newValue) => onChange(newValue)
          | _ => ()
          };
        ();
      };
      ();
    };

    let handleKeyDown = ev => {
      let _ =
        switch (onKeyDown) {
        | Some(onKeyDown) => onKeyDown(ev)
        | None => ()
        };

      let _ =
        keyEventHandled->React.Ref.setCurrent(
          ReactEvent.Keyboard.isDefaultPrevented(ev),
        );
      ();
    };

    let handleBlur = ev => {
      let _ = ReactEvent.Focus.persist(ev);
      let _ =
        if (Js.String.length(value.partial) > 0) {
          let _ = onChange({...value, partial: ""});
          ();
        };
      ();
    };

    /** Reuse the ref prop if one was passed in, otherwise use our own **/
    let inputRef = {
      let ownRef = React.useRef(Js.Nullable.null);
      switch (ref_->Js.Nullable.toOption) {
      | Some(inputRef) => inputRef
      | None => ownRef
      };
    };

    let handleContainerClick = _ => {
      let _ =
        inputRef
        ->React.Ref.current
        ->Js.Nullable.toOption
        ->Belt.Option.map(inputElem => {
            let _ =
              inputElem
              ->Webapi.Dom.Element.unsafeAsHtmlElement
              ->Webapi.Dom.HtmlElement.focus;
            ();
          });
      ();
    };

    <div
      onClick=handleContainerClick
      className={cn(["flex", "flex-col", Cn.unpack(className)])}>
      {value.commits
       ->Belt.Array.map(text =>
           <div key=text className=Cn.(fromList(["mr-3", "mb-3"]))>
             <span
               className={cn([
                 "font-sans",
                 "text-lightPrimary",
                 "font-medium",
                 "text-lg",
                 "border-b",
                 "border-white",
                 "border-opacity-50",
                 "inline-block",
                 "mr-3",
                 "mb-3",
               ])}>
               {React.string(text)}
             </span>
           </div>
         )
       ->React.array}
      <MaterialUi.TextField
        value={MaterialUi.TextField.Value.string(value.partial)}
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
    </div>;
  });
