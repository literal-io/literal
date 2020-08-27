open Styles;

[@decco]
type query = {alert: string};

[@react.component]
let make = (~query, ~mountDelay=250, ~clearDelay=5000) => {
  let (isVisible, setIsVisible) = React.useState(_ => false);

  let _ =
    React.useEffect1(
      () => {
        let timeouts =
          switch (query_decode(query)) {
          | Ok({alert}) =>
            Some((
              Js.Global.setTimeout(() => setIsVisible(_ => true), mountDelay),
              Js.Global.setTimeout(
                () => setIsVisible(_ => false),
                clearDelay + mountDelay,
              ),
            ))
          | _ => None
          };

        Some(
          () => {
            switch (timeouts) {
            | Some((mountTimeout, clearTimeout)) =>
              let _ = Js.Global.clearTimeout(mountTimeout);
              let _ = Js.Global.clearTimeout(clearTimeout);
              ();
            | _ => ()
            };
            ();
          },
        );
      },
      [|query|],
    );

  let handleClick = _ => setIsVisible(_ => false);

  switch (query_decode(query)) {
  | Belt.Result.Ok({alert}) =>
    <MaterialUi.Slide direction=`Up _in={isVisible}>
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
        <span
          className={cn(["sans-serif", "text-lightPrimary", "text-base"])}>
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
    </MaterialUi.Slide>
  | Belt.Result.Error(_) => React.null
  };
};
