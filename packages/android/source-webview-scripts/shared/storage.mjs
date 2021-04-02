const NAMESPACE = "__literal";

export const KEY_SERVICE_HIGHLIGHTER = "SERVICE_HIGHLIGHTER"
export const KEY_SERVICE_ANNOTATION_FOCUS_MANAGER = "SERVICE_ANNOTATION_FOCUS_MANAGER"

export const initialize = () => {
  window[NAMESPACE] = window[NAMESPACE] || {};
};

export const set = (key, value) => {
  window[NAMESPACE][key] = value;
};

export const get = (key) => window[NAMESPACE][key];
