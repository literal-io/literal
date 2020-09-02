open QueryRenderers_TagsFilter_GraphQL;
open Styles;

[@react.component]
let make = (~text, ~onTagResults, ~onTagClicked) => {
  let (_s, query) =
    ApolloHooks.useQuery(
      ~variables=FilterAnnotationCollections.makeVariables(~input=text, ()),
      FilterAnnotationCollections.definition,
    );

  let results =
    query.data
    ->Belt.Option.flatMap(d => d##listAnnotationCollections)
    ->Belt.Option.flatMap(d => d##items)
    ->Belt.Option.map(annotationCollections =>
        annotationCollections->Belt.Array.keepMap(annotationCollection =>
          annotationCollection->Belt.Option.flatMap(annotationCollection => {
            annotationCollection##label
            ->Belt.Option.flatMap(labels => labels->Belt.Array.get(0))
            ->Belt.Option.map(label =>
                Containers_NoteEditor_Base_Types.{
                  text: label,
                  id: Some(annotationCollection##id),
                }
              )
          })
        )
      )
    ->Belt.Option.getWithDefault([||]);

  let _ =
    React.useEffect1(
      () => {
        let _ = onTagResults(results);
        None;
      },
      [|query.data|],
    );

  let tags =
    results
    ->Belt.Array.map(tag => {
        <span
          onMouseDown={_ev => {
            let _ = onTagClicked(tag);
            ();
          }}
          key={tag.id->Belt.Option.getWithDefault(tag.text)}
          className={cn([
            "z-10",
            "font-sans",
            "text-lightSecondary",
            "italic",
            "underline",
            "font-medium",
            "pa-2",
            "mr-3",
          ])}>
          {React.string("#" ++ tag.text)}
        </span>
      })
    ->React.array;

  <div className={cn(["flex", "flex-row"])}> tags </div>;
};
