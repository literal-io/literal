let endpoint = Constants.Env.apiLiteralGraphQLAPIEndpointOutput;
let host = Node.(url(endpoint).hostname->hostnameToString);

[@decco]
type body = {
  query: string,
  operationName: string,
  variables: Js.Json.t,
};

let request = (~query, ~operationName, ~variables) => {
  let req = AWS.httpRequest(endpoint, Constants.Env.region);

  req.method_ = "POST";
  let _ = Js.Dict.set(req.headers, "host", host);
  let _ = Js.Dict.set(req.headers, "Content-Type", "application/json");
  req.body =
    body_encode({query, operationName, variables})->Js.Json.stringify;

  let signer = AWS.Signer.make(req, "appsync", true);
  let _ =
    AWS.Signer.addAuthorization(
      signer,
      AWS.Config.credentials,
      AWS.Util.getDate(),
    );

  req.host = host;

  Js.Promise.make((~resolve, ~reject) => {
    let httpRequest =
      Node.request(
        req,
        (. result) => {
          let _ =
            result->Node.on(
              `data(
                data => {
                  let _ = resolve(. Js.Json.parseExn(Node.toString(data)));
                  ();
                },
              ),
            );
          ();
        },
      );

    Node.write(httpRequest, req.body);
    Node.end_(httpRequest);
    ();
  });
};
