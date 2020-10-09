let styles = [%raw "require('./TagsList.module.css')"];

type tag = {
  id: option(string),
  href: option(string),
  text: string,
};

[@react.component]
let make = (~value, ~disabled=?) => {

  let handleChange = (~tag, ~idx) => {
    /** TODO **/
    ();
  };

  let tags =
    value
    ->Belt.Array.keep(({text}) => text != "recent")
    ->Belt.Array.mapWithIndex((idx, tag) =>
        <TagLinkAndInput
          key={tag.text}
          onChange={tag => handleChange(~tag, ~idx)}
          text={tag.text}
          href={tag.href}
          ?disabled
        />
      )
    ->React.array;

  <div className={Cn.fromList(["flex", "flex-col", "pt-8", "pb-4"])}>
    tags
  </div>;
};
