import path from "path";

const config = {
  mode: "production",
  optimization: {
    minimize: false,
  },
  entry: {
    GetAnnotation: new URL(
      "./get-annotation/index.mjs",
      import.meta.url
    ).pathname,
    HighlightAnnotationTarget: new URL(
      "./highlight-annotation-target/index.mjs",
      import.meta.url
    ).pathname,
  },
  output: {
    filename: "SourceWebView[name].js",
    path: new URL("../app/src/main/assets/", import.meta.url).pathname,
    iife: true
  },
};

export default config;
