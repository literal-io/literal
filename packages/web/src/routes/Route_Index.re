open Styles;

let manifesto = [|
  (
    "Separate signal from noise.",
    "Interacting with text elevates the reading experience beyond one of passive consumption and into one that requires analysis and critical thought. Annotations are of equal value to the text itself.",
  ),
  (
    "Contextualize and associate.",
    "Ideas do not exist in isolation. Ideas are threads that span otherwise disparate contexts. Annotations organized strictly by their original source is an artificial limitation. Create bridges, not silos.",
  ),
  (
    "Galleries for thought.",
    "The act of annotation is only the beginning. Ideas should be revisited and reexamined, tended as epistemological gardens.",
  ),
|];

let howItWorks = [|
  "Download the application.",
  "Create the annotation by highlighting or screenshotting the text that you want to annotate and sharing the result Literal.",
  "Create context for the annotation by associating metadata like source and notes.",
  "Create relationships for the annotation by associating tags. Tags are concepts, thoughts, and ideas, and annotations are the bricks that build them.",
  "Reflect as ideas evolve over time in response to new annotations and relationships between them.",
|];

let waitlistFormUrl = "https://docs.google.com/forms/d/1S6xFRp80nYYvPcz9oQYmiyOqLyXKHPZ0D2LofOxgWeo/edit";

[@react.component]
let default = () => {
  <div
    className={cn([
      "w-full",
      "h-full",
      "overflow-y-scroll",
      "flex",
      "flex-col",
      "bg-white",
    ])}>
    <div className={cn(["p-8", "bg-black", "max-w-lg", "m-auto"])}>
      <Svg
        icon=Svg.logo
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "w-12", "h-12", "block"])}
      />
      <h1
        className={cn([
          "text-white",
          "font-serif",
          "text-2xl",
          "leading-none",
          "mb-8",
          "mt-20",
        ])}>
        {React.string("Literal is a textual annotation management system.")}
      </h1>
      <a
        href=waitlistFormUrl
        className={cn([
          "text-white",
          "text-lg",
          "font-sans",
          "block",
          "mb-32",
        ])}>
        <Svg
          icon=Svg.arrowRight
          placeholderViewBox="0 0 24 24"
          className={cn(["pointer-events-none", "w-7", "h-7", "inline"])}
        />
        <span className={cn(["underline", "italic"])}>
          {React.string("Start Now")}
        </span>
      </a>
      {manifesto
       ->Belt.Array.map(((header, body)) =>
           <React.Fragment key=header>
             <h2
               className={cn([
                 "text-white",
                 "font-serif",
                 "text-xl",
                 "mb-2",
                 "underline",
               ])}>
               {React.string(header)}
             </h2>
             <p className={cn(["text-white", "font-sans", "mb-14", "block"])}>
               {React.string(body)}
             </p>
           </React.Fragment>
         )
       ->React.array}
      <hr className={cn(["bg-white", "opacity-50"])} />
      <h2
        className={cn([
          "text-white",
          "font-serif",
          "text-xl",
          "leading-none",
          "mt-14",
          "mb-12",
        ])}>
        {React.string("How Literal works")}
      </h2>
      <ul className={cn(["list-none", "list-inside"])}>
        {howItWorks
         ->Belt.Array.map(text =>
             <li key=text className={cn(["mb-6"])}>
               <div className={cn(["flex", "flex-row"])}>
                 <div
                   className={cn([
                     "border-white",
                     "border",
                     "w-3",
                     "h-3",
                     "mr-4",
                     "mt-1",
                     "flex-shrink-0",
                   ])}
                 />
                 <span className={cn(["text-white", "font-sans", "block"])}>
                   {React.string(text)}
                 </span>
               </div>
             </li>
           )
         ->React.array}
      </ul>
    </div>
    <div className={cn(["p-8", "bg-white", "max-w-lg", "w-full", "m-auto"])}>
      <a
        href=waitlistFormUrl
        className={cn([
          "text-black",
          "underline",
          "text-xl",
          "font-serif",
          "block",
          "mb-5",
        ])}>
        {React.string("Install")}
      </a>
      <a
        href="mailto:daniel@literal.io"
        className={cn([
          "text-black",
          "underline",
          "text-xl",
          "font-serif",
          "block",
          "mb-5",
        ])}>
        {React.string("Contact")}
      </a>
      <a
        href="https://github.com/javamonn/literal"
        className={cn([
          "text-black",
          "underline",
          "text-xl",
          "font-serif",
          "block",
        ])}>
        {React.string("Open Source")}
      </a>
    </div>
  </div>;
};

let page = "index.js"
