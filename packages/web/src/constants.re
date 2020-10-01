[@bs.module "./aws-exports.js"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";

type window;
[@bs.val] [@bs.scope "globalThis"] [@bs.return nullable]
external window: option(window) = "window";
let isBrowser = window->Js.Option.isSome;

module Env = {
  [@bs.val] external nodeEnv: string = "process.env.NODE_ENV";
  [@bs.val] external amplifyEnv: string = "process.env.AMPLIFY_ENV";
  [@bs.val] external amplitudeApiKey: string = "process.env.AMPLITUDE_API_KEY";
  [@bs.val] external sentryDsn: string = "process.env.SENTRY_DSN";
};

let apiOrigin =
  Env.amplifyEnv === "production"
    ? "https://literal.io" : "https://staging.literal.io";

%raw
{|
  const domains = awsAmplifyConfig.oauth.redirectSignIn.split(",")
  const domain =
    domains.find(d =>
      d.startsWith(
        process.env.NODE_ENV === 'development'
        ? 'http://'
        : apiOrigin
      )
    )
  awsAmplifyConfig.oauth.redirectSignIn = domain
  awsAmplifyConfig.oauth.redirectSignOut = domain
|};

let bowser = () =>
  Webapi.Dom.window
  ->Webapi.Dom.Window.navigator
  ->Bowser.asNavigator
  ->Bowser.userAgentGet
  ->Bowser.make;

let browser = () => bowser()->Bowser.getBrowser;

let serifFontFace = "Domine";
