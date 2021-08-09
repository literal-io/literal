[@bs.deriving jsConverter]
type shareFormat = [ | `Text | `Markdown];

[@react.component]
let make = (~hideDelete=false, ~onDelete, ~onShare) => {
  let (isMenuOpen, setIsMenuOpen) = React.useState(() => false);
  let (isDialogOpen, setIsDialogOpen) = React.useState(() => false);
  let (shareFormat, setShareFormat) = React.useState(() => `Text);
  let menuIconButtonRef = React.useRef(Js.Nullable.null);

  let handleToggleIsMenuOpen = () => setIsMenuOpen(open_ => !open_);
  let handleToggleIsDialogOpen = () => setIsDialogOpen(open_ => !open_);
  let handleShare = () => {
    handleToggleIsDialogOpen();
    onShare(~format=shareFormat);
  };
  let handleShareFormatChange = ev =>
    setShareFormat(shareFormat =>
      shareFormatFromJs(ReactEvent.Form.target(ev)##value)
      ->Belt.Option.getWithDefault(shareFormat)
    );

  <>
    <MaterialUi.Dialog
      fullWidth=true
      maxWidth=MaterialUi.Dialog.MaxWidth.xs
      _open=isDialogOpen
      onClose={(_, _) => {handleToggleIsDialogOpen()}}>
      <MaterialUi.DialogTitle
        classes={MaterialUi.DialogTitle.Classes.make(
          ~root=Cn.fromList(["font-sans"]),
          (),
        )}>
        <span className={Cn.fromList(["font-sans"])}>
          {React.string("Share Annotation")}
        </span>
      </MaterialUi.DialogTitle>
      <MaterialUi.DialogContent>
        <MaterialUi.FormControl
          component={MaterialUi.FormControl.Component.string("fieldset")}>
          <MaterialUi.FormLabel
            component={MaterialUi.FormLabel.Component.string("legend")}
            classes={MaterialUi.FormLabel.Classes.make(
              ~root=
                Cn.fromList(["font-sans", "text-darkDisabled", "text-sm"]),
              ~focused=Cn.fromList(["text-darkDisabled"]),
              (),
            )}>
            {React.string("Format")}
          </MaterialUi.FormLabel>
          <MaterialUi.RadioGroup
            value={MaterialUi_Types.Any(shareFormatToJs(shareFormat))}
            onChange=handleShareFormatChange>
            <MaterialUi.FormControlLabel
              value={MaterialUi_Types.Any(shareFormatToJs(`Text))}
              control={
                <MaterialUi.Radio
                  classes={MaterialUi.Radio.Classes.make(
                    ~checked=Cn.fromList(["text-darkPrimary"]),
                    (),
                  )}
                />
              }
              label={React.string("Text")}
              classes={MaterialUi.FormControlLabel.Classes.make(
                ~label={Cn.fromList(["font-sans"])},
                (),
              )}
            />
            <MaterialUi.FormControlLabel
              value={MaterialUi_Types.Any(shareFormatToJs(`Markdown))}
              control={
                <MaterialUi.Radio
                  classes={MaterialUi.Radio.Classes.make(
                    ~checked=Cn.fromList(["text-darkPrimary"]),
                    (),
                  )}
                />
              }
              label={React.string("Markdown")}
              classes={MaterialUi.FormControlLabel.Classes.make(
                ~label={Cn.fromList(["font-sans"])},
                (),
              )}
            />
          </MaterialUi.RadioGroup>
        </MaterialUi.FormControl>
      </MaterialUi.DialogContent>
      <MaterialUi.DialogActions>
        <MaterialUi.Button
          size=`Medium
          onClick={_ => handleToggleIsDialogOpen()}
          classes={MaterialUi.Button.Classes.make(
            ~label=Cn.fromList(["font-sans", "text-darkSecondary"]),
            (),
          )}
          variant=`Text>
          {React.string("Cancel")}
        </MaterialUi.Button>
        <MaterialUi.Button
          size=`Medium
          onClick={_ => handleShare()}
          classes={MaterialUi.Button.Classes.make(
            ~root=Cn.fromList(["ml-4", "bg-darkAccent"]),
            ~label=Cn.fromList(["font-sans", "text-lightPrimary"]),
            (),
          )}
          _TouchRippleProps={
            "classes": {
              "child": Cn.fromList(["bg-black"]),
              "rippleVisible": Cn.fromList(["opacity-75"]),
            },
          }
          variant=`Contained>
          {React.string("Share")}
        </MaterialUi.Button>
      </MaterialUi.DialogActions>
    </MaterialUi.Dialog>
    <MaterialUi.NoSsr>
      <MaterialUi.IconButton
        ref={menuIconButtonRef->ReactDOMRe.Ref.domRef}
        size=`Small
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => {handleToggleIsMenuOpen()}}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["p-0", "flex-initial", "flex-shrink-0"]),
          (),
        )}>
        <Svg
          style={ReactDOMRe.Style.make(
            ~width="1.75rem",
            ~height="1.75rem",
            (),
          )}
          className={Cn.fromList(["pointer-events-none", "opacity-75"])}
          icon=Svg.more
        />
      </MaterialUi.IconButton>
      <MaterialUi.Menu
        anchorEl={MaterialUi_Types.Any(menuIconButtonRef.current)}
        anchorReference=`AnchorEl
        _open=isMenuOpen
        onClose={(_, _) => {handleToggleIsMenuOpen()}}>
        <MaterialUi.MenuItem
          onClick={_ => {
            handleToggleIsMenuOpen();
            handleToggleIsDialogOpen();
          }}>
          <MaterialUi.ListItemIcon>
            <Svg
              className={Cn.fromList(["pointer-events-none", "opacity-50"])}
              style={ReactDOMRe.Style.make(
                ~width="1.25rem",
                ~height="1.25rem",
                (),
              )}
              icon=Svg.shareBlack
            />
          </MaterialUi.ListItemIcon>
          <MaterialUi.ListItemText
            classes={MaterialUi.ListItemText.Classes.make(
              ~primary=Cn.fromList(["font-sans"]),
              (),
            )}
            primary={React.string("Share")}
          />
        </MaterialUi.MenuItem>
        {hideDelete
           ? React.null
           : <MaterialUi.MenuItem onClick={_ => onDelete()}>
               <MaterialUi.ListItemIcon>
                 <Svg
                   className={Cn.fromList([
                     "pointer-events-none",
                     "opacity-50",
                   ])}
                   style={ReactDOMRe.Style.make(
                     ~width="1.25rem",
                     ~height="1.25rem",
                     (),
                   )}
                   icon=Svg.deleteBlack
                 />
               </MaterialUi.ListItemIcon>
               <MaterialUi.ListItemText
                 classes={MaterialUi.ListItemText.Classes.make(
                   ~primary=Cn.fromList(["font-sans"]),
                   (),
                 )}
                 primary={React.string("Delete")}
               />
             </MaterialUi.MenuItem>}
      </MaterialUi.Menu>
    </MaterialUi.NoSsr>
  </>;
};
