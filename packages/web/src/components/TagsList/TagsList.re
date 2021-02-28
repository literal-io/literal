let styles = [%raw "require('./TagsList.module.css')"];

[@react.component]
let make = (~value, ~onChange, ~disabled=?) => {
  let handleChange = (~newText, ~idx) => {
    let newTag =
      value
      ->Belt.Array.get(idx)
      ->Belt.Option.flatMap(tag =>
          newText->Belt.Option.map(text =>
            Containers_AnnotationEditor_Tag.{id: None, href: None, text}
          )
        );
    let newValue = Belt.Array.copy(value);
    let _ =
      Js.Array2.spliceInPlace(
        newValue,
        ~pos=idx,
        ~remove=1,
        ~add=
          switch (newTag) {
          | Some(newTag) => [|newTag|]
          | None => [||]
          },
      );
    onChange(newValue);
  };

  let tags =
    value
    ->Belt.Array.keep(({text}) =>
        text
        != Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel
      )
    ->Belt.Array.mapWithIndex((idx, tag) =>
        <li className={Cn.fromList(["mb-5"])} key={tag.text}>
          <TagLinkAndInput
            key={tag.text}
            onChange={newText => handleChange(~newText, ~idx)}
            text={tag.text}
            href={tag.href}
            ?disabled
          />
        </li>
      )
    ->React.array;

  <ul className={Cn.fromList(["pt-8", "pb-4"])}> tags </ul>;
};
