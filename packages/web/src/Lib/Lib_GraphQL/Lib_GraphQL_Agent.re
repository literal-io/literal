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

let makeId = (~identityId) => Constants.apiOrigin ++ "/agents/" ++ identityId;

let makeCache = (~identityId) =>
  Js.Json.object_(
    Js.Dict.fromList([
      ("__typename", "Agent"->Js.Json.string),
      ("id", makeId(~identityId)->Js.Json.string),
      ("email", Js.Json.null),
      ("email_sha1", Js.Json.null),
      ("type", "PERSON"->Js.Json.string),
      ("username", identityId->Js.Json.string),
      ("homepage", Js.Json.null),
      ("name", Js.Json.null),
      ("nickname", Js.Json.null),
    ]),
  );

let readCache = id => {
  Apollo.Client.readFragment(
    Providers_Apollo_Client.inst^,
    {id: "Agent:" ++ id, fragment: cacheAgentFragment},
  );
};
