[@bs.module "./aws-exports.js"]
external awsAmplifyConfig: AwsAmplify.Config.t = "default";

let browser = () =>
  Webapi.Dom.window
  ->Webapi.Dom.Window.navigator
  ->Bowser.asNavigator
  ->Bowser.userAgentGet
  ->Bowser.make
  ->Bowser.getBrowser;
