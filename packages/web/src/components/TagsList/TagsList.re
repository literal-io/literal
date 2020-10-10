let styles = [%raw "require('./TagsList.module.css')"];

type tag = {
  id: option(string),
  href: option(string),
  text: string,
};

[@react.component]
let make = (~value, ~disabled=?) => {
  let handleChange = (~tag, ~idx) =>
    /** TODO **/
    {
      ();
    };

  let tags =
    value
    ->Belt.Array.keep(({text}) => text != "recent")
    ->Belt.Array.mapWithIndex((idx, tag) =>
        <li className={Cn.fromList(["mb-5"])}>
          <TagLinkAndInput
            key={tag.text}
            onChange={tag => handleChange(~tag, ~idx)}
            text={tag.text}
            href={tag.href}
            ?disabled
          />
        </li>
      )
    ->React.array;

  <ul className={Cn.fromList(["pt-8", "pb-4"])}> tags </ul>;
};
