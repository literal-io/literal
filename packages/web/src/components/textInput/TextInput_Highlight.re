open Styles;
let styles = [%raw "require('./TextInput_Highlight.module.css')"];

[@react.component]
let make =
    (
      ~onTextChange,
      ~textValue,
      ~tagsValue,
      ~onTagsChange,
      ~className=?,
      ~inputClasses=[],
      ~autoFocus=?,
      ~placeholder=?,
    ) => {
  let tagsInputRef = React.useRef(Js.Nullable.null);
  let textInputRef = React.useRef(Js.Nullable.null);

  let (isFocused, setIsFocused) = React.useState(_ => false);

  let handleFocus = _ => {
    let _ = setIsFocused(_ => true);
    ();
  };
  let handleBlur = _ => {
    let _ = setIsFocused(_ => false);
    ();
  };

  let handleTagsKeyDown = ev => {
    let _ = ev->ReactEvent.Keyboard.persist;
    let handled =
      switch (ev->ReactEvent.Keyboard.keyCode) {
      | 8
          /*** backspace **/
          when
            Js.String.length(tagsValue.TextInput_Tags.Value.partial) === 0
            && Js.Array.length(tagsValue.commits) === 0 =>
        switch (textInputRef.current->Js.Nullable.toOption) {
        | Some(inputElem) =>
          let _ =
            inputElem
            ->Webapi.Dom.Element.unsafeAsHtmlElement
            ->Webapi.Dom.HtmlElement.focus;
          true;
        | None => false
        }
      | _ => false
      };
    let _ =
      if (handled) {
        let _ = ev->ReactEvent.Keyboard.preventDefault;
        let _ = ev->ReactEvent.Keyboard.stopPropagation;
        ();
      };
    ();
  };

  let handleTextKeyDown = ev => {
    let handled =
      switch (ev->ReactEvent.Keyboard.keyCode) {
      | 51 /*** # **/ =>
        switch (tagsInputRef.current->Js.Nullable.toOption) {
        | Some(inputElem) =>
          let _ =
            inputElem
            ->Webapi.Dom.Element.unsafeAsHtmlElement
            ->Webapi.Dom.HtmlElement.focus;
          true;
        | None => false
        }
      | _ => false
      };
    let _ =
      if (handled) {
        let _ = ev->ReactEvent.Keyboard.preventDefault;
        let _ = ev->ReactEvent.Keyboard.stopPropagation;
        ();
      };
    ();
  };

  <>
    <TextInput_Basic
      onFocus=handleFocus
      onBlur=handleBlur
      onChange=onTextChange
      value=textValue
      ?placeholder
      ?autoFocus
      inputProps={
        "onKeyDown": handleTextKeyDown,
        "inputRef": textInputRef->ReactDOMRe.Ref.domRef->Js.Option.some,
        "disableUnderline": false
      }
    />
    <TextInput_Tags
      onFocus=handleFocus
      onBlur=handleBlur
      onChange=onTagsChange
      onKeyDown=handleTagsKeyDown
      value=tagsValue
      className={cn(["pt-8", "pb-6"])}
      ref={tagsInputRef->ReactDOMRe.Ref.domRef}
    />
  </>;
};
