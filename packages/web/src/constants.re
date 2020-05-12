[@bs.module "./aws-exports.js"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";

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

let browser = () =>
  Webapi.Dom.window
  ->Webapi.Dom.Window.navigator
  ->Bowser.asNavigator
  ->Bowser.userAgentGet
  ->Bowser.make
  ->Bowser.getBrowser;
