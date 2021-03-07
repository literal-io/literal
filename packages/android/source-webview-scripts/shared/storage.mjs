const NAMESPACE = "__literal";

export const initialize = () => {
  window[NAMESPACE] = window[NAMESPACE] || {};
};

export const set = (key, value) => {
  window[NAMESPACE][key] = value;
};

export const get = (key) => window[NAMESPACE][key];
