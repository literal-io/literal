import "../app.css";
import "typeface-titillium-web";
import "typeface-domine";

import Head from "next/head";
import Router from "next/router";

import AuthenticationProvider from "../Providers/Providers_Authentication.js";
import ApolloProvider from "../Providers/Providers_Apollo.js";
import ErrorBoundary from "../components/ErrorBoundary/ErrorBoundary.js"
import { page, track } from "../services/Service_Analytics.js";

import * as React from "react";

if (global.window) {
  require("../webview.js").initialize();
}

export default function App({
  Component,
  pageProps,
  router: { asPath, route, query },
}) {
  // Next.js currently does not allow trailing slash in a route.
  // This is a client side redirect in case trailing slash occurs.
  // https://github.com/zeit/next.js/issues/5214
  if (pageProps.statusCode === 404 && asPath.length > 1) {
    const [path, query = ""] = asPath.split("?");

    if (path.endsWith("/")) {
      const asPathWithoutTrailingSlash =
        path.replace(/\/*$/gim, "") + (query ? `?${query}` : "");
      if (typeof window !== "undefined") {
        Router.replace(asPathWithoutTrailingSlash);
        return null;
      }
    }
  }

  React.useEffect(() => {
    window.requestIdleCallback(() => { 
      track(page({ route, asPath, query }));
    })
  }, [asPath, route, query]);

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
      <AuthenticationProvider.make>
        <ErrorBoundary.make>
          <ApolloProvider.make
            render={(rehydrated) => (
              <Component rehydrated={rehydrated} {...pageProps} />
            )}
          />
        </ErrorBoundary.make>
      </AuthenticationProvider.make>
    </>
  );
}
