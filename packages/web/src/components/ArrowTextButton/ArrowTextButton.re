type variant =
  | Link({href: string})
  | Button({onClick: unit => unit});

[@react.component]
let make = (~variant, ~label, ~action, ~className=?, ~style=?, ~iconStyle=?) => {
  let content =
    <>
      <Svg
        style=?iconStyle
        icon=Svg.arrowRight
        className={Cn.fromList(["w-6", "h-6"])}
      />
      <div>
        <span
          className={Cn.fromList([
            "italic",
            "border-b",
            "border-dotted",
            "border-lightPrimary",
            "block",
            "leading-none",
          ])}>
          {React.string(label)}
        </span>
      </div>
    </>;

  switch (variant) {
  | Link({href}) =>
    <Next.Link passHref=true href>
      <a
        ?style
        onClick={_ => {
          let _ =
            Service_Analytics.(track(Click({action, label: Some(label)})));
          ();
        }}
        className={Cn.fromList([
          "text-lightPrimary",
          "text-base",
          "font-sans",
          "flex",
          "flex-row",
          "items-center",
          Cn.take(className),
        ])}>
        content
      </a>
    </Next.Link>
  | Button({onClick}) =>
    <button
      ?style
      onClick={_ => {
        let _ = onClick();
        let _ =
          Service_Analytics.(track(Click({action, label: Some(label)})));
        ();
      }}
      className={Cn.fromList([
        "text-lightPrimary",
        "text-base",
        "font-sans",
        "flex",
        "flex-row",
        "items-center",
        Cn.take(className),
      ])}>
      content
    </button>
  };
};
