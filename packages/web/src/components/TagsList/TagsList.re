let styles = [%raw "require('./TagsList.module.css')"];

[@react.component]
let make = (~value, ~onChange, ~disabled=?, ~autoFocusAddTagInput=false) => {
  let handleChange = (~newText, ~idx) => {
    let currentTag: option(Containers_AnnotationEditor_Tag.t) =
      value->Belt.Array.get(idx);
    let newTag =
      currentTag->Belt.Option.flatMap(_ =>
        newText->Belt.Option.map(text =>
          Containers_AnnotationEditor_Tag.{id: None, href: None, text}
        )
      );
    switch (currentTag, newTag) {
    | (Some({text: currentTagText}), Some({text: newTagText} as newTag))
        when currentTagText != newTagText =>
      let newValue = Belt.Array.copy(value);
      let _ =
        Js.Array2.spliceInPlace(
          newValue,
          ~pos=idx,
          ~remove=1,
          ~add=[|newTag|],
        );
      onChange(newValue);
    | (Some(_), None) =>
      let newValue = Belt.Array.copy(value);
      let _ =
        Js.Array2.spliceInPlace(newValue, ~pos=idx, ~remove=1, ~add=[||]);
      onChange(newValue);
    | _ => ()
    };
  };

  let handleCreateTag = text => {
    let newTag = Containers_AnnotationEditor_Tag.{id: None, href: None, text};
    onChange(Belt.Array.concat(value, [|newTag|]));
    ();
  };

  let tags =
    value
    ->Belt.Array.mapWithIndex((idx, tag) =>
        if (tag.text
            == Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel) {
          React.null;
        } else {
          <li className={Cn.fromList(["mb-5"])} key={tag.text}>
            <TagLinkAndInput
              key={tag.text}
              onChange={newText => handleChange(~newText, ~idx)}
              text={tag.text}
              href={tag.href}
              ?disabled
            />
          </li>;
        }
      )
    ->Belt.Array.concat([|
        <li key="AddTagButtonAndInput">
          <AddTagInput
            onCreateTag=handleCreateTag
            autoFocus=autoFocusAddTagInput
          />
        </li>,
      |])
    ->React.array;

  <ul className={Cn.fromList(["pt-8", "pb-4"])}> tags </ul>;
};
