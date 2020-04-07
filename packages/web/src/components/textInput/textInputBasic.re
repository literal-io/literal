open Styles;

[@react.component]
let make = (~onChange, ~value) =>
  <MaterialUi.TextField
    onChange={ev => ev->ReactEvent.Form.target->(el => el##value)->onChange}
    value={`String(value)}
    fullWidth=true
    multiline=true
    _InputProps={
      "classes":
        MaterialUi.Input.Classes.(
          [
            Input(
              cn([
                "text-black",
                "font-serif",
                "text-lg",
                "leading-relaxed",
                "box-decoration-break-clone",
                "px-1",
                "textInputBasic_input"
              ]),
            ),
          ]
          ->to_obj
        ),
    }
  />;
