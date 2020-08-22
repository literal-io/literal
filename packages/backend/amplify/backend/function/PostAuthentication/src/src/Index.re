module CreateAgentMutation = [%graphql
  {|
  mutation CreateAgent($input: CreateAgentInput!) {
    createAgent(input: $input) {
      id
    }
  }
|}
];

let handleCreateProfile = (~username, ~email) => {
  let hashedEmail = {
    let hash = External_Node.Crypto.makeHash("sha1");
    let _ = External_Node.Crypto.update(hash, email);
    External_Node.Crypto.digest(hash, "hex");
  };

  let mutation =
    CreateAgentMutation.make(
      ~input={
        "id": "https://literal.io/agents/" ++ username,
        "type": `PERSON,
        "username": username,
        "email": Some([|Some(email)|]),
        "email_sha1": Some([|Some(hashedEmail)|]),
        "homepage": None,
        "name": None,
        "nickname": None
      },
      (),
    );
  Service_GraphQL.request(
    ~query=mutation##query,
    ~variables=mutation##variables,
    ~operationName="CreateAgent",
  );
};

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));
  let op =
    switch (event->External_Lambda.event_decode) {
    | Belt.Result.Ok(data) =>
      External_Lambda.(
        handleCreateProfile(
          ~username=data.userName,
          ~email=data.request.userAttributes.email
        )
      )
    | Belt.Result.Error(e) =>
      Js.log("Unable to decode event.");
      Js.Exn.raiseError(e.message);
    };

  op
  |> Js.Promise.then_(_ => Js.Promise.resolve(event))
  |> Js.Promise.catch(e => {
       Js.log2("Unable to handle event.", e);
       Js.Promise.resolve(event);
     });
};
