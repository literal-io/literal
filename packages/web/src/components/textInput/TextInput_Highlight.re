open Styles;
let styles = [%raw "require('./TextInput_Highlight.module.css')"];

[@react.component]
let make =
    (
      ~onTextChange,
      ~textValue,
      ~tagsValue,
      ~onTagsChange,
      ~inputClasses=[],
      ~autoFocus=?,
      ~placeholder=?,
    ) => {
  let (isFocused, setIsFocused) = React.useState(_ => false);

  let handleFocus = _ => {
    let _ = setIsFocused(_ => true);
    ();
  };
  let handleBlur = _ => {
    let _ = setIsFocused(_ => false);
    ();
  };

  <div
    className={cn([
      styles##underline,
      Cn.ifTrue(styles##underlineFocused, isFocused),
    ])}>
    <TextInput_Basic
      onFocus=handleFocus
      onBlur=handleBlur
      onChange=onTextChange
      value=textValue
      ?placeholder
      ?autoFocus
      inputProps={"disableUnderline": true}
    />
    <TextInput_Tags
      onFocus=handleFocus
      onBlur=handleBlur
      onChange=onTagsChange
      value=tagsValue
    />
  </div>;
};
