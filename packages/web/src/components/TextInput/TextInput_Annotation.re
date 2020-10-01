open Styles;
let styles = [%raw "require('./TextInput_Annotation.module.css')"];

[@react.component]
let make =
    (
      ~onTextChange,
      ~textValue,
      ~tagsValue,
      ~onTagsChange,
      ~disabled=?,
      ~autoFocus=?,
      ~placeholder=?,
      ~tagsInputRef=?,
    ) => {
  let textInputRef = React.useRef(Js.Nullable.null);

  /** Reuse the ref prop if one was passed in, otherwise use our own **/
  let tagsInputRef = {
    let ownRef = React.useRef(Js.Nullable.null);
    switch (tagsInputRef) {
    | Some(tagsInputRef) => tagsInputRef
    | None => ownRef
    };
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
      onChange=onTextChange
      value=textValue
      ?placeholder
      ?autoFocus
      ?disabled
      inputProps={
        "onKeyDown": handleTextKeyDown,
        "inputRef": textInputRef->ReactDOMRe.Ref.domRef->Js.Option.some,
        "disableUnderline": false,
      }
    />
    <TextInput_Tags
      ?disabled
      onChange=onTagsChange
      onKeyDown=handleTagsKeyDown
      value=tagsValue
      className={cn(["pt-8", "pb-4"])}
      ref={tagsInputRef->ReactDOMRe.Ref.domRef}
    />
  </>;
};
