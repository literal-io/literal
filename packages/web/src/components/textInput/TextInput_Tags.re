open Styles;

module Value = {
  type t = {
    commits: array(string),
    partial: string,
  };

  let empty = () => {commits: [||], partial: ""};
};

[@react.component]
let make =
  React.forwardRef(
    (~value, ~onChange, ~onFocus=?, ~onBlur=?, ~onKeyDown=?, ref_) => {
    let keyEventHandled = React.useRef(false);
    let (isFocused, setFocused) = React.useState(() => false);

    let handleChange = ev => {
      let data =
        ev
        ->ReactEvent.Form.nativeEvent
        ->(ev => ev##data)
        ->Js.Nullable.toOption;
      let inputType = ev->ReactEvent.Form.nativeEvent->(ev => ev##inputType);

      let newValue =
        switch (inputType, data) {
        | ("insertText", Some(insertedText)) =>
          Some(Value.{...value, partial: value.partial ++ insertedText})
        | ("deleteContentBackward", _)
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

    let handleFocus = ev => {
      let _ = setFocused(_ => true);
      let _ =
        switch (onFocus) {
        | Some(onFocus) => onFocus(ev)
        | None => ()
        };
      ();
    };

    let handleBlur = ev => {
      let _ = setFocused(_ => false);
      let _ =
        switch (onBlur) {
        | Some(onBlur) => onBlur(ev)
        | None => ()
        };
      ();
    };

    <div className={cn(["flex", "flex-row", "flex-wrap"])}>
      {value.commits
       ->Belt.Array.map(text =>
           <span
             key=text
             className={cn([
               "font-sans",
               "text-white",
               "italic",
               "underline",
               "font-medium",
               "mr-3",
             ])}>
             {React.string("#" ++ text)}
           </span>
         )
       ->React.array}
      <span
        className={cn([
          "font-sans",
          "text-white",
          "font-medium",
          "italic",
          "underline",
          Cn.ifTrue(
            cn(["absolute", "opacity-0"]),
            !(isFocused || Js.String2.length(value.partial) > 0),
          ),
        ])}>
        {React.string("#")}
      </span>
      <input
        type_="text"
        ref=?{
          ref_->Js.Nullable.toOption->Belt.Option.map(ReactDOMRe.Ref.domRef)
        }
        className={cn([
          "font-sans",
          "text-white",
          "font-medium",
          "italic",
          "bg-black",
          "underline",
          Cn.ifTrue(
            cn(["absolute", "opacity-0"]),
            !(isFocused || Js.String2.length(value.partial) > 0),
          ),
        ])}
        style={style(
          ~width=
            string_of_int(max(Js.String2.length(value.partial), 1)) ++ "ch",
          (),
        )}
        value={value.partial}
        onChange=handleChange
        onKeyUp=handleKeyUp
        onKeyDown=handleKeyDown
        onFocus=handleFocus
        onBlur=handleBlur
      />
    </div>;
  });
