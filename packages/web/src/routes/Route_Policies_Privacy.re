let md: string = [%raw "require('../static/markdown/privacy-policy.md')"];

[@react.component]
let default = () => {
  <div dangerouslySetInnerHTML={"__html": md} />;
};

let page = "policies/privacy.js";
