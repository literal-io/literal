module Client = {
  [@bs.deriving abstract]
  type authOptions = {
    [@bs.as "type"]
    type_: string,
    [@bs.optional]
    jwtToken: unit => Js.Promise.t(AwsAmplify.Auth.JwtToken.t),
    [@bs.optional]
    credentials: unit => Js.Promise.t(AwsAmplify.Auth.Credentials.t),
  };

  let authWithCognitoUserPools = (~jwtToken) =>
    authOptions(~type_="AMAZON_COGNITO_USER_POOLS", ~jwtToken, ());
  let authWithIAM = (~credentials) =>
    authOptions(~type_="AWS_IAM", ~credentials, ());

  type appSyncLinkOptions = {
    url: string,
    region: string,
    auth: authOptions,
    disableOffline: bool,
    mandatorySignIn: bool,
    complexObjectsCredentials:
      unit => Js.Promise.t(AwsAmplify.Auth.Credentials.t),
  };
  [@bs.module "aws-appsync"]
  external createAppSyncLink:
    appSyncLinkOptions => ReasonApolloTypes.apolloLink =
    "createAppSyncLink";

  [@bs.new] [@bs.module "aws-appsync"]
  external make: appSyncLinkOptions => ApolloClient.generatedApolloClient =
    "default";

  type makeWithCustomLinkOptions = {link: ReasonApolloTypes.apolloLink};
  [@bs.new] [@bs.module "aws-appsync"]
  external makeWithOptions:
    (appSyncLinkOptions, makeWithCustomLinkOptions) =>
    ApolloClient.generatedApolloClient =
    "default";
};

module Rehydrated = {
  type renderProps = {rehydrated: bool};
  [@bs.module "aws-appsync-react"] [@react.component]
  external make:
    (~render: renderProps => React.element=?, ~children: React.element=?) =>
    React.element =
    "Rehydrated";
};
