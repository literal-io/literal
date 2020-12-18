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

  Constants.isBrowser
    ? Amplitude.(getInstance()->logEventWithProperties(name, properties))
    : Js.log3("amplitude event noop:", name, properties);
};
