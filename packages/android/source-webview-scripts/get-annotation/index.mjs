import { makeAnnotationFromSelection } from "./model.mjs";

export default () => {
  const output = !window.getSelection().empty()
    ? makeAnnotationFromSelection({ selection: window.getSelection() })
    : null;

  window.getSelection().removeAllRanges();

  return output;
};
