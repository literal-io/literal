let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

let handler = event => {
  Js.log(Js.Json.stringifyAny(event));
}
