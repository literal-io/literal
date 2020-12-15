open Styles;

[@react.component]
let make = (~html) =>
  <div
    style={style(~backgroundColor="rgb(229,229,229)", ())}
    className={Cn.fromList([
      "w-full",
      "h-full",
      "overflow-y-scroll",
      "flex",
      "flex-col",
    ])}>
    <div
      className={cn([
        "p-8",
        "bg-black",
        "max-w-lg",
        "m-auto",
        "w-full",
      ])}>
      <div className={Cn.fromList(["mb-16", "flex", "flex-row"])}>
        <Next.Link _as={Routes.Index.path()} href=Routes.Index.staticPath>
          <a>
            <Svg
              icon=Svg.logo
              placeholderViewBox="0 0 24 24"
              className={cn(["pointer-events-none", "w-12", "h-12", "block"])}
            />
          </a>
        </Next.Link>
      </div>
      <Markdown html />
    </div>
  </div>;
