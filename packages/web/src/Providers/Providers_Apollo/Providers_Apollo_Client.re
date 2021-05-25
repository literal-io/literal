let fragmentTypes = [%raw "require('../../../fragment-types.json')"];

let appSyncCache =
  ApolloInMemoryCache.createInMemoryCache(
    ~fragmentMatcher=
      ApolloInMemoryCache.introspectionFragmentMatcher({
        "introspectionQueryResultData": fragmentTypes,
      }),
    (),
  );

let makeClientFromUser = user => {
  let auth =
    switch (user) {
    | Providers_Authentication_User.GuestUser({credentials}) =>
      credentials
      ->Providers_Apollo_AppSync.makeGuestClientAuthOptions
      ->Js.Option.some
    | SignedInUser({cognitoUserSession})
    | SignedInUserMergingIdentites({cognitoUserSession}) =>
      cognitoUserSession
      ->Providers_Apollo_AppSync.makeAuthenticatedClientAuthOptions
      ->Js.Option.some
    | SignedOutPromptAuthentication
    | Unknown => None
    };

  let appSyncLinkOptions =
    Providers_Apollo_AppSync.makeAppSyncLinkOptions(~auth);
  let appSyncLink = Providers_Apollo_AppSync.(makeLink(appSyncLinkOptions));

  AwsAppSync.Client.(
    makeWithOptions(
      appSyncLinkOptions,
      {
        link:
          Some(
            ApolloLinks.from([|
              Providers_Apollo_AnalyticsLink.link,
              appSyncLink,
            |]),
          ),
        cache: Some(appSyncCache),
      },
    )
  );
};

// FIXME: This should get exposed via react context.
let inst = ref(makeClientFromUser(Providers_Authentication_User.Unknown));
