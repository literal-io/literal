import {
  get as storageGet,
  initialize as storageInitialize,
} from "../shared/storage.mjs";
const ANNOTATION = process.env.PARAM_ANNOTATION;

export default () => {
  storageInitialize();

  const ranges = storageGet("annotationRanges");
  if (!ranges || !ranges[ANNOTATION.id]) {
    console.error("[Literal] Unable to find range for annotation.");
    return;
  }

  const boundingBox = ranges[ANNOTATION.id].getBoundingClientRect();
  return {
    left: boundingBox.left,
    top: boundingBox.top,
    right: boundingBox.right,
    bottom: boundingBox.bottom,
  };
};
