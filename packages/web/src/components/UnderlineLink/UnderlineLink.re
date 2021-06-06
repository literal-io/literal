[@react.component]
let make = (~text, ~href) =>
  <Next.Link passHref=true href>
    <a
      className={Cn.fromList([
        "text-lightSecondary",
        "text-base",
        "font-sans",
        "italic",
        "border-b",
        "border-dotted",
        "border-lightPrimary",
        "leading-none",
      ])}>
      {React.string(text)}
    </a>
  </Next.Link>;
