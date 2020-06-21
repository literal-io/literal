open Externals;
open Lib.Constants;

let endpoint = Env.apiLiteralGraphQLAPIEndpointOutput;
let host = Node.Url.(url(endpoint).hostname->hostnameToString);

[@decco]
type body = {
  query: string,
  operationName: string,
  variables: Js.Json.t,
};

let request = (~query, ~operationName, ~variables) => {
  let req = AWS.httpRequest(endpoint, Env.region);

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
      Node.Https.request(
        AWS.toNodeRequest(req),
        (. result) => {
          let _ =
            result->Node.Https.on(
              `data(
                data => {
                  let _ =
                    resolve(. Js.Json.parseExn(Node.Https.toString(data)));
                  ();
                },
              ),
            );
          ();
        },
      );

    Node.Https.write(httpRequest, req.body);
    Node.Https.end_(httpRequest);
    ();
  });
};
