let cacheAgentFragment =
  ApolloClient.gql(.
    {|
  fragment CacheAgent on Agent {
    id
    email
    email_sha1
    type_: type
    username
    homepage
    name
    nickname
  }
|},
  );

let makeId = (~currentUser) =>
  Constants.apiOrigin
  ++ "/agents/"
  ++ currentUser->AwsAmplify.Auth.CurrentUserInfo.username;

let makeCache = (~currentUser) => {
  let email = AwsAmplify.Auth.CurrentUserInfo.(currentUser->attributes->email);

  Lib_GraphQL.makeHash(~digest="SHA-1", email)
  |> Js.Promise.then_(hashedEmail =>
       Js.Json.object_(
         Js.Dict.fromList([
           ("id", makeId(~currentUser)->Js.Json.string),
           ("email", [|email->Js.Json.string|]->Js.Json.array),
           ("email_sha1", [|hashedEmail->Js.Json.string|]->Js.Json.array),
           ("type", "PERSON"->Js.Json.string),
           (
             "username",
             currentUser
             ->AwsAmplify.Auth.CurrentUserInfo.username
             ->Js.Json.string,
           ),
           ("homepage", Js.Json.null),
           ("name", Js.Json.null),
           ("nickname", Js.Json.null),
         ]),
       )
       ->Js.Promise.resolve
     );
};

let readCache = id => {
  Apollo.Client.readFragment(
    Providers_Apollo.client,
    {id: "Agent:" ++ id, fragment: cacheAgentFragment},
  );
};
