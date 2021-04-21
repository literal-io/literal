let styles = [%raw "require('./AuthenticationFields.module.css')"];

type authenticationType =
  | SignIn
  | SignUp;

[@react.component]
let make = (~onChange, ~onSubmit, ~type_) => {
  let (email, setEmail) = React.useState(() => "");
  let (emailError, setEmailError) = React.useState(() => None);
  let (password, setPassword) = React.useState(() => "");
  let (passwordError, setPasswordError) = React.useState(() => None);

  let passwordInputRef = React.useRef(Js.Nullable.null);

  let _ =
    React.useEffect2(
      () => {
        onChange((email, password));
        None;
      },
      (email, password),
    );

  let validateEmail = email =>
    if (Js.String2.length(email) < 3) {
      setEmailError(_ => Some("Not a valid email address."));
      false;
    } else {
      setEmailError(_ => None);
      true;
    };

  let validatePassword = password =>
    if (Js.String2.length(password) < 8) {
      setPasswordError(_ => Some("Password must be at least 8 characters."));
      false;
    } else {
      setPasswordError(_ => None);
      true;
    };

  let handleChange = (setter, validator, error, ev) => {
    let newValue = ev->ReactEvent.Form.target->(el => el##value);
    let _ =
      error->Belt.Option.forEach(_ => {
        let _ = validator(newValue);
        ();
      });
    setter(_ => newValue);
  };

  let handleKeyUpEmailInput = ev => {
    let keyCode = ReactEvent.Keyboard.keyCode(ev);
    if (keyCode == 13 && validateEmail(email)) {
      passwordInputRef.current
      ->Js.Nullable.toOption
      ->Belt.Option.forEach(inputElem =>
          inputElem
          ->Webapi.Dom.Element.unsafeAsHtmlElement
          ->Webapi.Dom.HtmlElement.focus
        );
    };
  };
  let handleKeyUpPasswordInput = ev => {
    let keyCode = ReactEvent.Keyboard.keyCode(ev);
    if (keyCode == 13 && validatePassword(password)) {
      onSubmit();
    };
  };

  <>
    <MaterialUi.TextField
      _InputProps={
        "classes":
          MaterialUi.Input.Classes.make(
            ~root=
              Cn.fromList([
                "bg-darkAccent",
                "border-b",
                "border-lightSecondary",
                "border-dotted",
              ]),
            ~focused=Cn.fromList(["border-none"]),
            ~input=
              Cn.fromList([
                "font-sans",
                "text-lightPrimary",
                "font-medium",
                "text-base",
              ]),
            ~underline=Cn.fromList([styles##underline]),
            (),
          ),
      }
      _InputLabelProps={
        "classes":
          MaterialUi.InputLabel.Classes.make(
            ~root=Cn.fromList(["font-sans", "text-lightDisabled"]),
            ~focused=Cn.fromList(["text-lightSecondary"]),
            (),
          ),
      }
      error={Belt.Option.isSome(emailError)}
      helperText=?{
        emailError->Belt.Option.map(emailError => React.string(emailError))
      }
      inputProps={
        "enterKeyHint": "next",
        "autoComplete": "email",
        "onKeyUp": handleKeyUpEmailInput,
      }
      label={React.string("Email Address")}
      fullWidth=true
      _type="email"
      value={MaterialUi.TextField.Value.string(email)}
      onChange={handleChange(setEmail, validateEmail, emailError)}
      variant=`Filled
      autoFocus=true
    />
    <MaterialUi.TextField
      classes={MaterialUi.TextField.Classes.make(
        ~root=Cn.fromList(["mt-4"]),
        (),
      )}
      _InputProps={
        "classes":
          MaterialUi.Input.Classes.make(
            ~root=
              Cn.fromList([
                "bg-darkAccent",
                "border-b",
                "border-lightSecondary",
                "border-dotted",
              ]),
            ~focused=Cn.fromList(["border-none"]),
            ~input=
              Cn.fromList([
                "font-sans",
                "text-lightPrimary",
                "font-medium",
                "text-base",
              ]),
            ~error=Cn.fromList([styles##underlineError]),
            ~underline=Cn.fromList([styles##underline]),
            (),
          ),
      }
      _FormHelperTextProps={
        "classes":
          MaterialUi.FormHelperText.Classes.make(
            ~error=Cn.fromList(["font-sans"]),
            (),
          ),
      }
      _InputLabelProps={
        "classes":
          MaterialUi.InputLabel.Classes.make(
            ~root=Cn.fromList(["font-sans", "text-lightDisabled"]),
            ~focused=Cn.fromList(["text-lightSecondary"]),
            (),
          ),
      }
      label={React.string("Password")}
      error={Belt.Option.isSome(passwordError)}
      helperText=?{
        passwordError->Belt.Option.map(passwordError =>
          React.string(passwordError)
        )
      }
      fullWidth=true
      _type="password"
      value={MaterialUi.TextField.Value.string(password)}
      onChange={handleChange(setPassword, validatePassword, passwordError)}
      inputProps={
        "enterKeyHint": "done",
        "ref": passwordInputRef->ReactDOMRe.Ref.domRef,
        "onKeyUp": handleKeyUpPasswordInput,
        "autoComplete":
          switch (type_) {
          | SignIn => "password"
          | SignUp => "new-password"
          },
      }
      variant=`Filled
    />
  </>;
};
