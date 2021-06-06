[@react.component]
let default = () => {
  <>
    <Head>
      {Head.makeMeta(
         ~title="Install / Literal",
         ~description=
           "Install the application. Literal supports annotations made wherever you read.",
         (),
       )}
    </Head>
    <header className={Cn.fromList(["pt-12", "px-6", "sm:px-12"])}>
      <Next.Link href="/" passHref=true>
        <a>
          <Svg
            icon=Svg.logo
            className={Cn.fromList(["w-12", "h-12", "block"])}
          />
        </a>
      </Next.Link>
    </header>
    <main
      className={Cn.fromList([
        "px-6",
        "sm:px-12",
        "bg-black",
        "w-full",
        "py-24",
      ])}>
      <h1
        className={Cn.fromList([
          "text-lightPrimary",
          "font-serif",
          "text-2xl",
          "leading-tight",
          "mb-8",
        ])}>
        {React.string("Install the application.")}
      </h1>
      <p
        className={Cn.fromList(["font-sans", "text-lightPrimary", "text-lg"])}>
        {React.string("Literal supports annotations made wherever you read.")}
      </p>
      <div
        className={Cn.fromList([
          "mt-24",
          "flex",
          "flex-col",
          "items-start",
          "pb-4",
        ])}>
        <PromptIconButton
          label="Android"
          href="https://play.google.com/store/apps/details?id=io.literal"
          icon=Svg.android
          className={Cn.fromList(["mb-8"])}
        />
        <PromptIconButton
          label="iOS"
          icon=Svg.apple
          href="https://docs.google.com/forms/d/e/1FAIpQLSep8MKGHvJ9bN3TlB2chnrwd6HnzrhxOYE7iSTnUAoti5DR2g/viewform?usp=sf_link"
          className={Cn.fromList(["mb-8"])}
        />
        <PromptIconButton
          label="Web Extension"
          icon=Svg.language
          href="https://docs.google.com/forms/d/e/1FAIpQLSfvlr1DKw1YgL_G72Y36ViduG7k1T2_dj3_xS4k2XUwLMe2kg/viewform?usp=sf_link"
        />
      </div>
    </main>
  </>;
};

let page = "start.js";
