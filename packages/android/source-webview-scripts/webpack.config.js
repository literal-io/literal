const resolvePath = (path) => new URL(path, import.meta.url).pathname;

const config = {
  mode: "production",
  optimization: {
    minimize: false,
  },
  entry: {
    GetAnnotation: resolvePath("./get-annotation/index.mjs"),
    AnnotationRenderer: resolvePath("./annotation-renderer/index.mjs"),
    GetAnnotationBoundingBox: resolvePath(
      "./get-annotation-bounding-box/index.mjs"
    ),
  },
  output: {
    filename: "SourceWebView[name].js",
    path: resolvePath("./dist"),
    library: "Literal",
  },
};

export default config;
