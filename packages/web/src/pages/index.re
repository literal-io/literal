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

[@react.component]
let default = () => {
  <div className={cn(["w-full", "h-full", "overflow-y-scroll"])}>
    <div className={cn(["p-8", "bg-black"])}>
      <Svg
        icon=Svg.logo
        placeholderViewBox="0 0 24 24"
        className={cn(["pointer-events-none", "w-12", "h-12", "block"])}
      />
      <h1
        className={cn([
          "text-white",
          "font-serif",
          "text-xl",
          "leading-none",
          "mb-20",
          "mt-12",
        ])}>
        {React.string("Literal is a textual annotation management system.")}
      </h1>
      {manifesto
       ->Belt.Array.map(((header, body)) =>
           <>
             <h2
               className={cn([
                 "text-white",
                 "font-serif",
                 "text-lg",
                 "mb-2",
                 "underline",
               ])}>
               {React.string(header)}
             </h2>
             <p className={cn(["text-white", "font-sans", "mb-14", "block"])}>
               {React.string(body)}
             </p>
           </>
         )
       ->React.array}
      <hr className={cn(["bg-white", "opacity-50"])} />
      <h2
        className={cn([
          "text-white",
          "font-serif",
          "text-lg",
          "leading-none",
          "mt-14",
          "mb-12",
        ])}>
        {React.string("How Literal works")}
      </h2>
      <ul className={cn(["list-none", "list-inside"])}>
        {howItWorks
         ->Belt.Array.map(text =>
             <li className={cn(["mb-6"])}>
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
    <div className={cn(["p-8", "bg-white"])}>
      <a
        className={cn([
          "text-black",
          "underline",
          "text-lg",
          "font-serif",
          "block",
          "mb-5",
        ])}>
        {React.string("Install")}
      </a>
      <a
        className={cn([
          "text-black",
          "underline",
          "text-lg",
          "font-serif",
          "block",
          "mb-5",
        ])}>
        {React.string("Contact")}
      </a>
      <a
        className={cn([
          "text-black",
          "underline",
          "text-lg",
          "font-serif",
          "block",
        ])}>
        {React.string("Open Source")}
      </a>
    </div>
  </div>;
};
