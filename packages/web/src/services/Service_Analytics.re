let isEnabled = () =>
  Raw.isBrowser()
  && !
       Webview.JavascriptInterface.isFlavorFoss()
       ->Belt.Option.getWithDefault(false);

let initialize = () =>
  if (isEnabled()) {
    Amplitude.(getInstance()->init(Constants.Env.amplitudeApiKey));
  };

[@decco.encode]
type graphqlOperation = {
  variables: Js.Json.t,
  operationName: string,
};

[@decco.encode]
type page = {
  route: string,
  asPath: string,
  query: option(Js.Json.t),
};

[@decco.encode]
type click = {
  label: option(string),
  action: string,
};

[@decco.encode]
type error = {
  sentryEventId: string,
  [@decco.key "type"]
  type_: string,
};

[@bs.deriving accessors]
type event =
  | GraphQLOperation(graphqlOperation)
  | Page(page)
  | Click(click)
  | Error(error);

let track = event => {
  let (name, properties) =
    switch (event) {
    | GraphQLOperation(p) => (
        "GRAPH_QL_OPERATION",
        graphqlOperation_encode(p),
      )
    | Page(p) => ("PAGE", page_encode(p))
    | Click(p) => ("CLICK", click_encode(p))
    | Error(p) => ("ERROR", error_encode(p))
    };

  let handled = ref(false);
  if (Raw.isBrowser()) {
    let data =
      Js.Json.object_(
        Js.Dict.fromList([
          ("type", name->Js.Json.string),
          ("properties", properties),
        ]),
      );
    handled :=
      Webview.(
        postMessage(WebEvent.make(~type_="ANALYTICS_LOG_EVENT", ~data, ()))
      );
    let _ =
      if (! handled^
          && !
               Webview.JavascriptInterface.isFlavorFoss()
               ->Belt.Option.getWithDefault(false)) {
        Amplitude.(getInstance()->logEventWithProperties(name, properties));
      };
    ();
  };

  if (! handled^) {
    Js.log3("[Service_Analytics]", "track", event);
  };
};

let setUserId = userId => {
  let handled = ref(false);
  if (Raw.isBrowser()) {
    handled :=
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="ANALYTICS_SET_USER_ID",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([
                  (
                    "userId",
                    userId
                    ->Belt.Option.map(Js.Json.string)
                    ->Belt.Option.getWithDefault(Js.Json.null),
                  ),
                ]),
              ),
            (),
          ),
        )
      );

    let _ =
      if (! handled^
          && !
               Webview.JavascriptInterface.isFlavorFoss()
               ->Belt.Option.getWithDefault(false)) {
        Amplitude.(getInstance()->setUserId(userId));
        handled := true;
      };
    ();
  };

  if (! handled^) {
    Js.log3("[Service_Analytics]", "setUserId", userId);
  };
  ();
};
