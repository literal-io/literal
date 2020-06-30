[@bs.module "./aws-exports.js"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";

type window;
[@bs.val] [@bs.scope "globalThis"] [@bs.return nullable]
external window: option(window) = "window";
let isBrowser = window->Js.Option.isSome;

%raw
{|
  const domains = awsAmplifyConfig.oauth.redirectSignIn.split(",")
  const domain =
    domains.find(d =>
      d.startsWith(
        process.env.NODE_ENV === 'development'
        ? 'http://'
        : 'https://'
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

let serifFontFace = "Prata";
