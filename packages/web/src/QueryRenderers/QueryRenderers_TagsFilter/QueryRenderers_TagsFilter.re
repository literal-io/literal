open QueryRenderers_TagsFilter_GraphQL;
open Styles;

type labelBeginsWith = {
  hasResults: bool,
  text: string,
};

[@react.component]
let make = (~currentUser, ~pendingValue, ~tags, ~onTagResults, ~onTagClicked) => {
  let previousLabelBeginsWith = React.useRef({hasResults: true, text: ""});

  let nextLabelBeginsWithText =
    !previousLabelBeginsWith.current.hasResults
    && Js.String2.startsWith(
         pendingValue,
         previousLabelBeginsWith.current.text,
       )
      ? previousLabelBeginsWith.current.text : pendingValue;

  let (_s, query) =
    ApolloHooks.useQuery(
      ~variables=
        AnnotationCollectionLabelAutocomplete.makeVariables(
          ~labelBeginsWith=nextLabelBeginsWithText,
          ~creatorUsername=
            AwsAmplify.Auth.CurrentUserInfo.username(currentUser),
          (),
        ),
      ~skip=Js.String2.length(nextLabelBeginsWithText) == 0,
      AnnotationCollectionLabelAutocomplete.definition,
    );

  let results =
    query.data
    ->Belt.Option.flatMap(d => d##listAnnotationCollectionsByLabel)
    ->Belt.Option.flatMap(d => d##items)
    ->Belt.Option.map(annotationCollections =>
        annotationCollections->Belt.Array.keepMap(annotationCollection =>
          annotationCollection->Belt.Option.flatMap(annotationCollection => {
            switch (
              tags->Belt.Array.getBy(commit =>
                commit.text == annotationCollection##label
              )
            ) {
            | Some(_) => None
            | None =>
              Some(
                Containers_AnnotationEditor_Tag.{
                  text: annotationCollection##label,
                  id: Some(annotationCollection##id),
                  href: None,
                },
              )
            }
          })
        )
      )
    ->Belt.Option.getWithDefault([||]);

  let _ =
    React.useEffect1(
      () => {
        let _ = onTagResults(results);
        previousLabelBeginsWith.current = {
          ...previousLabelBeginsWith.current,
          hasResults: Js.Array2.length(results) > 0,
        };
        None;
      },
      [|query.data|],
    );

  let _ =
    React.useEffect1(
      () => {
        previousLabelBeginsWith.current = {
          ...previousLabelBeginsWith.current,
          text: nextLabelBeginsWithText,
        };
        None;
      },
      [|nextLabelBeginsWithText|],
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
            "pa-4",
            "mr-3",
            "block",
          ])}>
          {React.string(tag.text)}
        </span>
      })
    ->React.array;

  <div className={cn(["flex", "flex-row", "h-6"])}> tags </div>;
};
