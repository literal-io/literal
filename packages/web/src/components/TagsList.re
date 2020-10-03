type tag = {
  id: option(string),
  href: option(string),
  text: string,
};

[@react.component]
let make = (~value, ~disabled=?) => {
  value
  ->Belt.Array.keep(({text}) => text != "recent")
  ->Belt.Array.map(({text, href}) => {
      let tag =
        <MaterialUi.Button
          variant=`Text
          fullWidth=true
          ?disabled
          onClick={_ => {
            let _ =
              Service_Analytics.(
                track(Click({action: "tag", label: Some(text)}))
              );
            ();
          }}
          classes={MaterialUi.Button.Classes.make(
            ~root=Cn.fromList(["mb-1"]),
            ~text=
              Cn.fromList([
                "font-sans",
                "text-lightPrimary",
                "font-medium",
                "text-lg",
                "normal-case",
                "justify-start",
              ]),
            (),
          )}>
          <span
            className={Cn.fromList([
              "border-b",
              "border-white",
              "border-opacity-50",
            ])}>
            {React.string(text)}
          </span>
        </MaterialUi.Button>;

      switch (href) {
      | Some(href) =>
        <Next.Link
          key=href
          _as=href
          href=Routes.CreatorsIdAnnotationCollectionsId.staticPath
          passHref=true>
          tag
        </Next.Link>
      | None => tag
      };
    })
  ->React.array;
};
