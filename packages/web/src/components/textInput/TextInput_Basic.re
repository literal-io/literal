open Styles;

let styles = [%raw "require('./textInputBasic.module.css')"];
[@react.component]
let make =
  React.forwardRef(
    (
      ~onChange: string => unit,
      ~onFocus=?,
      ~onBlur=?,
      ~inputClasses=[],
      ~value,
      ~autoFocus=?,
      ~placeholder=?,
      ~label=?,
      ~inputProps: {
         .
         "disableUnderline": bool,
         "onKeyDown": ReactEvent.Keyboard.t => unit,
         "inputRef": option(ReactDOMRe.domRef),
       }=Js.Obj.empty(),
      ref_,
    ) => {
    let (fontFaceLoaded, setFontFaceLoaded) =
      React.useState(() =>
        FontFaceSet.(inst->check("normal 1rem " ++ Constants.serifFontFace))
      );

    let _ =
      React.useEffect0(() => {
        let onLoadingDone = ref(None);
        onLoadingDone :=
          Some(
            ev => {
              let isLoaded =
                FontFaceSet.LoadingDoneEvent.(
                  ev
                  ->unsafeOfEvent
                  ->fontfaces
                  ->Belt.Array.getBy(fontface =>
                      family(fontface) === Constants.serifFontFace
                    )
                  ->Belt.Option.map(fontface => status(fontface) === "loaded")
                  ->Belt.Option.getWithDefault(false)
                );
              let _ =
                if (isLoaded && !fontFaceLoaded) {
                  let _ = setFontFaceLoaded(_ => isLoaded);
                  let _ =
                    FontFaceSet.inst
                    |> FontFaceSet.removeEventListener(
                         "loadingdone",
                         Belt.Option.getExn(onLoadingDone^),
                       );
                  ();
                };
              ();
            },
          );
        let _ =
          if (!fontFaceLoaded) {
            let _ =
              FontFaceSet.inst
              |> FontFaceSet.addEventListener(
                   "loadingdone",
                   Belt.Option.getExn(onLoadingDone^),
                 );

            ();
          };
        Some(
          () => {
            let _ =
              FontFaceSet.inst
              |> FontFaceSet.removeEventListener(
                   "loadingdone",
                   Belt.Option.getExn(onLoadingDone^),
                 );
            ();
          },
        );
      });

    <MaterialUi.TextField
      key={fontFaceLoaded ? "loaded" : "loading"}
      ?label
      ?placeholder
      ?autoFocus
      ?onFocus
      ?onBlur
      onChange={ev => ev->ReactEvent.Form.target->(el => el##value)->onChange}
      value={`String(value)}
      fullWidth=true
      multiline=true
      ref=?{
        ref_->Js.Nullable.toOption->Belt.Option.map(ReactDOMRe.Ref.domRef)
      }
      _InputProps={Raw.merge(
        {
          "classes":
            MaterialUi.Input.Classes.(
              List.concat([
                [
                  Input(
                    cn([
                      "text-white",
                      "font-serif",
                      "text-lg",
                      "leading-relaxed",
                      "box-decoration-break-clone",
                      "px-1",
                      "textInputBasic_input",
                    ]),
                  ),
                  Underline(styles##underline),
                ],
                inputClasses,
              ])
              ->to_obj
            ),
        },
        inputProps,
      )}
      _InputLabelProps={
        "classes":
          MaterialUi.InputLabel.Classes.(
            [Root("text-white"), Focused("text-white")]->to_obj
          ),
      }
    />;
  });
