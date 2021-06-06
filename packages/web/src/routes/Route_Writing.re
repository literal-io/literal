let data: Js.Json.t = [%raw "require('../static/json/writing.json')"];
module Data = {
  [@decco.decode]
  type item = {
    title: string,
    subtitle: string,
    filename: string,
  };

  [@decco.decode]
  type t = array(item);

  let decode = t_decode;
};

[@react.component]
let default = (~items) =>
  <>
    <Head>
      {Head.makeMeta(
         ~title="Writing / Literal",
         ~description=
           "Updates, ideas, and inspiration from Literal, the textual annotation management system.",
         (),
       )}
    </Head>
    <header
      className={Cn.fromList([
        "mt-12",
        "flex",
        "flex-col",
        "px-6",
        "sm:px-12",
      ])}>
      <Next.Link _as={Routes.Index.path()} href=Routes.Index.staticPath>
        <a>
          <Svg
            icon=Svg.logo
            className={Cn.fromList([
              "pointer-events-none",
              "w-12",
              "h-12",
              "block",
            ])}
          />
        </a>
      </Next.Link>
      <h1
        className={Cn.fromList([
          "font-serif",
          "text-lightPrimary",
          "text-2xl",
          "flex-1",
          "py-24",
        ])}>
        {React.string("Writing")}
      </h1>
    </header>
    <main
      className={Cn.fromList(["px-6", "sm:px-12"])}
      style={ReactDOM.Style.make(~maxWidth="750px", ())}>
      <ul>
        {items
         ->Belt.Array.map((Data.{title, subtitle, filename}) =>
             <Next.Link
               _as={Routes.WritingId.path(~id=filename)}
               href=Routes.WritingId.staticPath>
               <a>
                 <li
                   className={Cn.fromList([
                     "border-b",
                     "border-lightDisabled",
                     "border-dotted",
                     "py-6",
                   ])}>
                   <h3
                     className={Cn.fromList([
                       "font-serif",
                       "text-lightPrimary",
                       "text-xl",
                     ])}>
                     {React.string(title)}
                   </h3>
                   <p
                     className={Cn.fromList([
                       "font-sans",
                       "text-lightSecondary",
                       "text-base",
                     ])}>
                     {React.string(subtitle)}
                   </p>
                 </li>
               </a>
             </Next.Link>
           )
         ->React.array}
      </ul>
    </main>
  </>;

let getStaticProps =
    (_: Next.getStaticPropsContext({.}))
    : Next.getStaticPropsResult({. "items": array(Data.item)}) =>
  switch (Data.decode(data)) {
  | Belt.Result.Ok(data) => {
      props: {
        "items": data,
      },
    }
  | Belt.Result.Error(e) =>
    Js.Exn.raiseError("Unable to decode data: " ++ e.message)
  };

let page = "writing.js";
