[@react.component]
let make = (~onCreateFromText, ~onCreateFromFile) =>
  <div
    className={Cn.fromList([
      "flex",
      "flex-row",
      "justify-center",
      "items-start",
      "flex-auto",
      "pt-20",
    ])}>
    <div
      className={Cn.fromList([
        "flex",
        "flex-row",
        "flex-auto",
        "justify-center",
        "items-center",
        "pb-4",
        "mx-6",
        "border-b",
        "border-white-o50",
      ])}>
      <MaterialUi.IconButton
        size=`Medium
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => {
          let _ =
            Service_Analytics.(
              track(Click({action: "create from text", label: None}))
            );
          onCreateFromText();
        }}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["mr-20", "p-8"]),
          (),
        )}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
          icon=Svg.textFields
        />
      </MaterialUi.IconButton>
      <MaterialUi.IconButton
        size=`Medium
        edge=MaterialUi.IconButton.Edge.start
        onClick={_ => {
          let _ =
            Service_Analytics.(
              track(Click({action: "create from file", label: None}))
            );
          onCreateFromFile();
        }}
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        classes={MaterialUi.IconButton.Classes.make(
          ~root=Cn.fromList(["p-8"]),
          (),
        )}>
        <Svg
          placeholderViewBox="0 0 24 24"
          className={Cn.fromList(["w-16", "h-16", "pointer-events-none"])}
          icon=Svg.textSnippet
        />
      </MaterialUi.IconButton>
    </div>
  </div>;
