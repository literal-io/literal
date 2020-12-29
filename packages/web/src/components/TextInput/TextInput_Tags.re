let styles = [%raw "require('./TextInput_Tags.module.css')"];

[@react.component]
let make =
  React.forwardRef(
    (~value, ~onValueChange, ~className=?, ~disabled=?, ~onValueCommit, ref_) => {
    /** Reuse the ref prop if one was passed in, otherwise use our own **/
    let inputRef = {
      let ownRef = React.useRef(Js.Nullable.null);
      switch (ref_->Js.Nullable.toOption) {
      | Some(inputRef) => inputRef
      | None => ownRef
      };
    };
    let (isFocused, setIsFocused) = React.useState(_ => false);

    let handleChange = ev => {
      let newValue =
        ev->ReactEvent.Form.nativeEvent->(ev => ev##target##value);

      if (Js.String2.endsWith(newValue, "\n") && Js.String.length(value) > 0) {
        let _ = ReactEvent.Form.preventDefault(ev);
        onValueCommit(value);
      } else {
        onValueChange(newValue);
      };
    };

    let handleBlur = _ => {
      if (Js.String.length(value) > 0) {
        let _ = onValueChange("");
        ();
      };
      let _ = setIsFocused(_ => false);
      ();
    };

    let handleFocus = _ => {
      let _ = setIsFocused(_ => true);
      ();
    };

    <MaterialUi.TextField
      ?disabled
      value={MaterialUi.TextField.Value.string(value)}
      onChange=handleChange
      fullWidth=true
      multiline=true
      classes={MaterialUi.TextField.Classes.make(
        ~root=
          Cn.fromList([
            "border-t",
            "border-dotted",
            "py-2",
            "transition-colors",
            "duration-300",
            "ease-in-out",
            isFocused ? "border-lightPrimary" : "border-lightDisabled",
            Cn.take(className),
          ]),
        (),
      )}
      _InputProps={
        "disableUnderline": true,
        "startAdornment":
          <MaterialUi.InputAdornment position=`Start>
            <Svg
              icon=Svg.label
              placeholderViewBox="0 0 24 24"
              className={Cn.fromList([
                "w-6",
                "h-6",
                "mr-2",
                "transition-opacity",
                "duration-300",
                "ease-in-out",
                isFocused ? "opacity-75" : "opacity-50",
              ])}
            />
          </MaterialUi.InputAdornment>,
        "classes":
          MaterialUi.Input.Classes.make(
            ~input=
              Cn.(
                fromList([
                  "font-sans",
                  "text-lightPrimary",
                  "font-medium",
                  "text-base",
                ])
              ),
            ~underline=Cn.(fromList([styles##underline])),
            (),
          ),
      }
      inputProps={
        "onBlur": handleBlur,
        "onFocus": handleFocus,
        "ref": inputRef->ReactDOMRe.Ref.domRef,
      }
    />;
  });
