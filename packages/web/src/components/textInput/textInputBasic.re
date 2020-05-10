open Styles;

let styles = [%raw "require('./textInputBasic.module.css')"];

[@react.component]
let make = (~onChange, ~label=?, ~value) =>
  <MaterialUi.TextField
    ?label
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
                "text-white",
                "font-serif",
                "text-lg",
                "italic",
                "leading-relaxed",
                "box-decoration-break-clone",
                "px-1",
                "textInputBasic_input",
              ]),
            ),
            Underline(styles##underline),
          ]
          ->to_obj
        ),
    }
    _InputLabelProps={
      "classes":
        MaterialUi.InputLabel.Classes.(
          [Root("text-white"), Focused("text-white")]->to_obj
        ),
    }
  />;
