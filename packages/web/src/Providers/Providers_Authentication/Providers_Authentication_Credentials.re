[@decco]
type tokens = {
  idToken: string,
  refreshToken: string,
  accessToken: string,
};

[@decco]
type awsCredentials = {
  accessKeyId: string,
  secretAccessKey: string,
  identityId: string,
  sessionToken: string,
};

external fromAmplifyCredentials: AwsAmplify.Credentials.t => awsCredentials =
  "%identity";

external toAmplifyCredentials: awsCredentials => AwsAmplify.Credentials.t =
  "%identity";

[@decco]
type t = {
  tokens: option(tokens),
  awsCredentials: option(awsCredentials),
};

let decode = t_decode;
let encode = t_encode;
