import "../app.css";
import "@fontsource/roboto-mono/latin.css";
import "@fontsource/domine/latin.css";

import Router from "next/router";

import AuthenticationProvider from "../Providers/Providers_Authentication.js";
import ApolloProvider from "../Providers/Providers_Apollo.js";
import ErrorBoundary from "../components/ErrorBoundary/ErrorBoundary.js";
import Head from "../components/Head.js";
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
    const idleCallback = window.requestIdleCallback || window.setImmediate;
    idleCallback(() => {
      track(page({ route, asPath, query }));
    });
  }, [asPath, route, query]);

  return (
    <>
      <Head.make />
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
