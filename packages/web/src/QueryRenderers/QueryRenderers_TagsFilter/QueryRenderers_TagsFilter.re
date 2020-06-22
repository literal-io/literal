open QueryRenderers_TagsFilter_GraphQL;
open Styles;

[@react.component]
let make = (~text, ~onTagResults, ~onTagClicked) => {
  let (_s, query) =
    ApolloHooks.useQuery(
      ~variables=FilterTags.makeVariables(~input=text, ()),
      FilterTags.definition,
    );

  let results =
    query.data
    ->Belt.Option.flatMap(d => d##listTags)
    ->Belt.Option.flatMap(d => d##items)
    ->Belt.Option.map(d => d->Belt.Array.keepMap(d => d));

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
    ->Belt.Option.map(d =>
        d
        ->Belt.Array.map(tag =>
            <span
              onMouseDown={_ev => {
                let _ = onTagClicked(tag);
                ();
              }}
              key=tag##text
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
              {React.string("#" ++ tag##text)}
            </span>
          )
        ->React.array
      )
    ->Belt.Option.getWithDefault(React.null);

  <div className={cn(["flex", "flex-row"])}> tags </div>;
};
