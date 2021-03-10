import { makeAnnotationFromSelection } from "./model.mjs";

export default () => {
  const output =
    window.getSelection().rangeCount > 0
      ? makeAnnotationFromSelection({ selection: window.getSelection() })
      : null;

  window.getSelection().removeAllRanges();

  return output;
};
