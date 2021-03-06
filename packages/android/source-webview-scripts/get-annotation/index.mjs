import { makeAnnotationFromSelection } from "./model.mjs";

(() => {
  const output = !window.getSelection().empty()
    ? makeAnnotationFromSelection({ selection: window.getSelection() })
    : null;

  window.getSelection().removeAllRanges();

  return output;
})();
