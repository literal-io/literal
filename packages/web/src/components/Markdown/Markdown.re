let styles = [%raw "require('./Markdown.module.css')"];

[@react.component]
let make = (~html) => {
  <article className={styles##md} dangerouslySetInnerHTML={"__html": html} />;
};
