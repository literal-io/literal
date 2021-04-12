let _ =
  Constants.isBrowser
    ? Amplitude.(getInstance()->init(Constants.Env.amplitudeApiKey)) : ();

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

  if (Constants.isBrowser) {
    let data =
      Js.Json.object_(
        Js.Dict.fromList([
          ("type", name->Js.Json.string),
          ("properties", properties),
        ]),
      );
    let dispatched =
      Webview.(
        postMessage(WebEvent.make(~type_="ANALYTICS_LOG_EVENT", ~data, ()))
      );
    let _ =
      if (!dispatched) {
        Amplitude.(getInstance()->logEventWithProperties(name, properties));
      };
    ();
  } else {
    Js.log3("amplitude event noop:", name, properties);
  };
};

let setUserId = userId => {
  if (Constants.isBrowser) {
    let dispatched =
      Webview.(
        postMessage(
          WebEvent.make(
            ~type_="ANALYTICS_SET_USER_ID",
            ~data=
              Js.Json.object_(
                Js.Dict.fromList([("userId", userId->Js.Json.string)]),
              ),
            (),
          ),
        )
      );

    let _ =
      if (!dispatched) {
        Amplitude.(getInstance()->setUserId(userId));
      };
    ();
  };
  ();
};
