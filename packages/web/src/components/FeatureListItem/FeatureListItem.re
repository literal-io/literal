[@react.component]
let make = (~enabled, ~title, ~description, ~className=?) =>
  <li className={Cn.fromList(["flex", "flex-row", Cn.take(className)])}>
    <FeatureIndicator enabled className={Cn.fromList(["mt-1"])} />
    <div className={Cn.fromList(["flex", "flex-col"])}>
      <h3
        className={Cn.fromList([
          "font-serif",
          "text-lg",
          "mb-1",
          enabled ? "text-lightPrimary" : "text-lightDisabled",
        ])}>
        {React.string(title)}
      </h3>
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightPrimary",
          enabled ? "text-lightSecondary" : "text-lightDisabled",
        ])}>
        {React.string(description)}
      </p>
    </div>
  </li>;
