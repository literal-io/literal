let styles = [%raw "require('./AddTagInput.module.css')"];

[@react.component]
let make = (~onCreateTag, ~autoFocus=false, ~onFocus=?, ~onBlur=?) => {
  let (pendingValue, setPendingValue) = React.useState(_ => "");
  let inputRef = React.useRef(Js.Nullable.null);

  let handleChange = ev => {
    let newValue = ev->ReactEvent.Form.target->(el => el##value);
    setPendingValue(_ => newValue);
  };
  let handleBlur = _ => {
    setPendingValue(_ => "");
    let _ = onBlur->Belt.Option.forEach(cb => cb());
    ();
  };
  let handleFocus = _ => {
    let _ = onFocus->Belt.Option.forEach(cb => cb());
    ();
  }
  let handleKeyUp = ev => {
    let keyCode = ReactEvent.Keyboard.keyCode(ev);
    if (keyCode == 13 && Js.String2.length(pendingValue) > 0) {
      onCreateTag(pendingValue);
      setPendingValue(_ => "");
      inputRef.current
      ->Js.Nullable.toOption
      ->Belt.Option.forEach(inputElem =>
          inputElem
          ->Webapi.Dom.Element.unsafeAsHtmlElement
          ->Webapi.Dom.HtmlElement.blur
        );
    } else if ((keyCode == 8 || keyCode == 13)
               && Js.String2.length(pendingValue) == 0) {
      inputRef.current
      ->Js.Nullable.toOption
      ->Belt.Option.forEach(inputElem =>
          inputElem
          ->Webapi.Dom.Element.unsafeAsHtmlElement
          ->Webapi.Dom.HtmlElement.blur
        );
    };
  };

  <MaterialUi.TextField
    _InputProps={
      "classes":
        MaterialUi.Input.Classes.make(
          ~input=
            Cn.fromList([
              "font-sans",
              "text-base",
              "font-medium",
              "text-lightPrimary",
              Cn.on(
                Cn.fromList(["italic", "uppercase", "text-sm"]),
                Js.String2.length(pendingValue) == 0,
              ),
            ]),
          ~underline=Cn.fromList([styles##underline]),
          (),
        ),
      "startAdornment":
        <Svg
          className={Cn.fromList([
            "pointer-events-none",
            "opacity-50",
            "mr-2",
          ])}
          style={ReactDOMRe.Style.make(~width="1rem", ~height="1rem", ())}
          icon=Svg.addCircle
        />,
    }
    inputProps={
      "ref": inputRef->ReactDOMRe.Ref.domRef,
      "onKeyUp": handleKeyUp,
    }
    placeholder="Add Tag"
    value={MaterialUi.TextField.Value.string(pendingValue)}
    fullWidth=true
    onChange=handleChange
    onBlur=handleBlur
    onFocus=handleFocus
    autoFocus
  />;
};
