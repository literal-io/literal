[@decco]
type callerContext = {
  awsSdkVersion: string,
  clientId: string,
};

[@decco]
type userAttributes = {
  sub: string,
  identities: string,
  [@decco.key "cognito:user_status"]
  cognitoUserStatus: string,
  email: string,
};

[@decco]
type request = {
  userAttributes,
  newDeviceUsed: bool,
};

[@decco]
type event = {
  version: string,
  region: string,
  userPoolId: string,
  userName: string,
  callerContext,
  triggerSource: string,
  request,
  response: Js.Json.t,
};
