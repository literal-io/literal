import "../app.css";

import * as React from "react";

const Provider = require("../provider").make;

export default function App({ Component, pageProps }) {
  return (
    <Provider>
      <Component {...pageProps} />
    </Provider>
  );
}
