[@react.component]
let default = () => {
  <div
    style={ReactDOM.Style.make(~backgroundColor="rgb(229, 229, 229)", ())}
    className={Cn.fromList([
      "w-full",
      "h-full",
      "overflow-y-scroll",
      "flex",
      "flex-col",
    ])}>
    <main
      className={Cn.fromList([
        "p-8",
        "bg-black",
        "max-w-lg",
        "m-auto",
        "min-h-full",
        "w-full",
      ])}>
      <h1
        className={Cn.fromList([
          "text-lightPrimary",
          "font-serif",
          "text-xl",
          "leading-tight",
          "mb-8",
        ])}>
        {React.string("1. Install the application.")}
      </h1>
      <p
        className={Cn.fromList([
          "font-sans",
          "text-lightSecondary",
          "text-lg",
          "mb-16",
        ])}>
        {React.string(
           "Literal supports annotations made wherever you read.",
         )}
      </p>
      <div
        className={Cn.fromList([
          "flex",
          "flex-col",
          "justify-center",
          "items-center",
          "pb-4",
          "mx-6",
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
  </div>;
};

let page = "start.js";
