import "../app.css";

import * as React from "react";

if (global.window) {
  require('../webview.js').initialize();
}

const Provider = require("../provider").make;

export default function App({ Component, pageProps }) {
  return (
    <Provider>
      <Component {...pageProps} />
    </Provider>
  );
}
