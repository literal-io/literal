import "core-js/index.js";
import "regenerator-runtime/runtime.js";

import {
  get as storageGet,
  initialize as storageInitialize,
} from "../shared/storage.mjs";
const ANNOTATION = JSON.parse(process.env.PARAM_ANNOTATION);

export default () => {
  storageInitialize();

  const ranges = storageGet("annotationRanges");
  if (!ranges || !ranges[ANNOTATION.id]) {
    console.error("[Literal] Unable to find range for annotation.");
    return;
  }

  const boundingBox = ranges[ANNOTATION.id].getBoundingClientRect();
  return {
    left: boundingBox.left * window.devicePixelRatio,
    top: boundingBox.top * window.devicePixelRatio,
    right: boundingBox.right * window.devicePixelRatio,
    bottom: boundingBox.bottom * window.devicePixelRatio,
  };
};
