let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

module CreateProfileMutation = [%graphql
  {|
  mutation CreateProfile($input: CreateProfileInput!) {
    createProfile(input: $input) {
      id
    }
  }
|}
];

let handleCreateProfile = username => {
  let mutation =
    CreateProfileMutation.make(
      ~input={
        "id": UUID.makeV4(),
        "createdAt": None,
        "isOnboarded": false,
        "owner": username,
      },
      (),
    );
  let op =
    AwsAmplify.Api.graphqlOperation(
      ~query=mutation##query,
      ~variables=mutation##variables,
    );
  AwsAmplify.Api.(graphql(inst, op));
};

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));
  let op =
    switch (event->Lambda.event_decode) {
    | Belt.Result.Ok(event) => handleCreateProfile(event.userName)
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
