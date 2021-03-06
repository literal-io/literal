[@react.component]
let make = (~href, ~title, ~description) =>
  <Next.Link passHref=true href scroll=true>
    <a
      className={Cn.fromList([
        "block",
        "border",
        "border-dotted",
        "border-lightDisabled",
        "p-4",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "flex-row",
          "items-center",
          "justify-between",
        ])}>
        <div className={Cn.fromList(["flex", "flex-col"])}>
          <h3
            className={Cn.fromList([
              "font-serif",
              "text-lightPrimary",
              "mb-2",
            ])}>
            {React.string(title)}
          </h3>
          <p
            className={Cn.fromList([
              "font-sans",
              "text-lightSecondary",
              "leading-tight",
            ])}>
            {React.string(description)}
          </p>
        </div>
        <Svg
          icon=Svg.arrowRight
          className={Cn.fromList([
            "pointer-events-none",
            "w-12",
            "h-12",
            "opacity-75",
          ])}
        />
      </div>
    </a>
  </Next.Link>;
