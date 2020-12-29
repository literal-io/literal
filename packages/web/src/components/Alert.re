open Styles;

[@decco]
type query = {alert: string};

[@react.component]
let make = (~urlSearchParams, ~visibleDelay=250, ~clearDelay=5000, ~onClear=?) => {
  let visibleTimeoutId = React.useRef(None);
  let clearTimeoutId = React.useRef(None);
  let (isVisible, setIsVisible) = React.useState(_ => false);

  let _ =
    React.useEffect1(
      () => {
        Webapi.Url.URLSearchParams.get("alert", urlSearchParams)
        ->Belt.Option.forEach(_ => {
            visibleTimeoutId.current =
              Some(
                Js.Global.setTimeout(
                  () => {
                    setIsVisible(_ => true);
                    clearTimeoutId.current =
                      Some(
                        Js.Global.setTimeout(
                          () => {
                            setIsVisible(_ => false);
                            onClear->Belt.Option.forEach(cb => cb());
                          },
                          clearDelay,
                        ),
                      );
                  },
                  visibleDelay,
                ),
              )
          });

        Some(
          () => {
            let timeouts = [|
              clearTimeoutId.current,
              visibleTimeoutId.current,
            |];

            let _ =
              timeouts->Belt.Array.map(t => {
                let _ = t->Belt.Option.forEach(Js.Global.clearTimeout);
                ();
              });

            clearTimeoutId.current = None;
            visibleTimeoutId.current = None;
            ();
          },
        );
      },
      [|urlSearchParams|],
    );

  let handleClick = _ => setIsVisible(_ => false);

  let alert =
    Webapi.Url.URLSearchParams.get("alert", urlSearchParams)
    ->Belt.Option.getWithDefault("");

  <MaterialUi.Slide direction=`Up _in=isVisible>
    <div
      onClick=handleClick
      className={cn([
        "absolute",
        "inset-x-0",
        "bottom-0",
        "mx-6",
        "my-4",
        "mh-16",
        "z-20",
        "flex",
        "flex-row",
        "items-center",
        "justify-start",
        "px-6",
        "py-6",
      ])}>
      <span className={cn(["sans-serif", "text-lightPrimary", "text-xs"])}>
        {React.string(alert)}
      </span>
      <div
        className={cn(["absolute", "bg-black", "border", "border-white"])}
        style={ReactDOMRe.Style.make(
          ~bottom=rem(0.),
          ~right=rem(0.),
          ~left=rem(0.25),
          ~top=rem(0.25),
          ~zIndex="-1",
          (),
        )}
      />
      <div
        style={ReactDOMRe.Style.make(
          ~top=rem(0.),
          ~left=rem(0.),
          ~right=rem(0.25),
          ~bottom=rem(0.25),
          ~zIndex="-1",
          (),
        )}
        className={cn(["absolute", "bg-black", "border", "border-white"])}
      />
    </div>
  </MaterialUi.Slide>;
};
