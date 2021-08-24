import "core-js/index.js";
import "regenerator-runtime/runtime.js";

import { Messenger } from "./messenger.mjs";
import { Highlighter } from "./highlighter.mjs";
import { Renderer } from "./renderer.mjs";
import { AnnotationFocusManager } from "./annotation-focus-manager.mjs";
import {
  get as storageGet,
  set as storageSet,
  initialize as storageInitialize,
  KEY_SERVICE_HIGHLIGHTER,
  KEY_SERVICE_ANNOTATION_FOCUS_MANAGER,
} from "../shared/storage.mjs";

const HIGHLIGHT_CLASS_NAME = "literal-highlight";

// Initial parameters set during script injection. Note that effective values may change over the
// course of script execution due to message handling.
const ANNOTATIONS = JSON.parse(process.env.PARAM_ANNOTATIONS);
const FOCUSED_ANNOTATION_ID = process.env.PARAM_FOCUSED_ANNOTATION_ID;

const messenger = new Messenger({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const highlighter = new Highlighter({
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const annotationFocusManager = new AnnotationFocusManager({
  messenger,
  highlighter,
  highlightClassName: HIGHLIGHT_CLASS_NAME,
});
const renderer = new Renderer({
  messenger,
  highlighter,
  annotationFocusManager,
});

const onDocumentReady = (cb) => {
  if (document.readyState === "complete") {
    cb();
  } else {
    document.addEventListener("DOMContentLoaded", () => {
      cb();
    });
  }
};

export default () =>
  onDocumentReady(async () => {
    storageInitialize();
    storageSet(KEY_SERVICE_ANNOTATION_FOCUS_MANAGER, annotationFocusManager);
    storageSet(KEY_SERVICE_HIGHLIGHTER, highlighter);

    if (storageGet("hasInitialized")) {
      console.log(
        "[Literal] Expected uninitialized DOM, but found it already initialized: noop-ing."
      );
      return;
    }
    storageSet("hasInitialized", true);

    try {
      const rangeByAnnotationId = await renderer.render(ANNOTATIONS);
      if (Object.values(rangeByAnnotationId).length !== ANNOTATIONS.length) {
        messenger.postMessage({
          type: "ANNOTATION_RENDERER_FAILED_TO_INITIALIZE",
        });
        return;
      }

      renderer.onInitialAnnotationsRendered();
      annotationFocusManager.onAnnotationsRendered({
        annotations: ANNOTATIONS,
        focusedAnnotationId: FOCUSED_ANNOTATION_ID,
        initialRender: true,
      });

      messenger.postMessage({
        type: "ANNOTATION_RENDERER_INITIALIZED",
      });
    } catch (ex) {
      messenger.postMessage({
        type: "ANNOTATION_RENDERER_FAILED_TO_INITIALIZE",
      });
    }
  });
