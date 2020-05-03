import "../app.css";
import Head from "next/head";

import * as React from "react";

if (global.window) {
  require("../webview.js").initialize();
}

const Provider = require("../provider").make;

export default function App({ Component, pageProps }) {
  return (
    <>
      <Head>
        <title>Literal</title>
        <meta
          key="viewport"
          name="viewport"
          content="initial-scale=1.0, width=device-width"
        />
        <link rel="icon" type="image/png" href="/favicon-32.png" />
      </Head>
      <Provider>
        <Component {...pageProps} />
      </Provider>
    </>
  );
}
