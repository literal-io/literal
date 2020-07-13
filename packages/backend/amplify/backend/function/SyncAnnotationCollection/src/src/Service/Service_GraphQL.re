let endpoint = Constants.Env.apiLiteralGraphQLAPIEndpointOutput;
let host = External_Node.Url.(url(endpoint).hostname->hostnameToString);

[@decco]
type body = {
  query: string,
  operationName: string,
  variables: Js.Json.t,
};

let request = (~query, ~operationName, ~variables) => {
  let req = External_AWS.httpRequest(endpoint, Constants.Env.region);

  req.method_ = "POST";
  let _ = Js.Dict.set(req.headers, "host", host);
  let _ = Js.Dict.set(req.headers, "Content-Type", "application/json");
  req.body =
    body_encode({query, operationName, variables})->Js.Json.stringify;

  let signer = External_AWS.Signer.make(req, "appsync", true);
  let _ =
    External_AWS.Signer.addAuthorization(
      signer,
      External_AWS.Config.credentials,
      External_AWS.Util.getDate(),
    );

  req.host = host;

  Js.Promise.make((~resolve, ~reject) => {
    let httpRequest =
      External_Node.Https.request(
        External_AWS.toNodeRequest(req),
        (. result) => {
          let _ =
            result->External_Node.Https.on(
              `data(
                data => {
                  let _ =
                    resolve(.
                      Js.Json.parseExn(External_Node.Https.toString(data)),
                    );
                  ();
                },
              ),
            );
          ();
        },
      );

    External_Node.Https.write(httpRequest, req.body);
    External_Node.Https.end_(httpRequest);
    ();
  });
};
