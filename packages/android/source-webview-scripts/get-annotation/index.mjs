import { makeAnnotationFromSelection } from "./model.mjs";
import {
  get as storageGet,
  KEY_SERVICE_HIGHLIGHTER,
  KEY_SERVICE_ANNOTATION_FOCUS_MANAGER,
} from "../shared/storage.mjs";

export default () => {
  const highlighterService = storageGet(KEY_SERVICE_HIGHLIGHTER);
  const annotationFocusManagerService = storageGet(
    KEY_SERVICE_ANNOTATION_FOCUS_MANAGER
  );

  if (!highlighterService || !annotationFocusManagerService) {
    console.error(
      "[Literal] Expected annotationFocusManager and highlighter services, but found null.",
      highlighterService,
      annotationFocusManagerService
    );

    return null;
  }

  annotationFocusManagerService._handleBlurAnnotation({ disableNotify: true })
  highlighterService.removeHighlights()

  const output =
    window.getSelection().rangeCount > 0
      ? makeAnnotationFromSelection({ selection: window.getSelection() })
      : null;

  window.getSelection().removeAllRanges();

  return output;
};
