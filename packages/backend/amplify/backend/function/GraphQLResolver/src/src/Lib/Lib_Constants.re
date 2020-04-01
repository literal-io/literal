module Env = {
  [@bs.val] external region: string = "process.env.REGION";
  [@bs.val]
  external apiLiteralGraphQLAPIEndpointOutput: string =
    "process.env.API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT";
  [@bs.val]
  external storageLiteralStorageBucketName: string =
    "process.env.STORAGE_LITERAL_STORAGE_BUCKETNAME";
  [@bs.val]
  external authLiteralAuthUserPoolId: string =
    "process.env.AUTH_LITERALAUTH_USERPOOLID";
  [@bs.val]
  external authLiteralAuthUserPooWebClientId: string =
    "process.env.AUTH_LITERALAUTH_USERPOOLWEBCLIENTID";
  [@bs.val]
  external highlightBoundingBoxDetectorAPI: string =
    "process.env.HIGHLIGHT_BOUNDING_BOX_DETECTOR_API";
};

let gcloudServiceAccountFilename =
  Node.Path.resolve(Externals_Node.dirname, "../gcloud-service-account.json");

let highlightBoundingBoxScoreThreshold = 0.3;

let awsAmplifyConfig =
  AwsAmplify.Config.make(
    ~projectRegion=Env.region,
    ~userPoolsId=Env.authLiteralAuthUserPoolId,
    ~userPoolsWebClientId=Env.authLiteralAuthUserPooWebClientId,
    ~appSyncRegion=Env.region,
    ~appSyncGraphqlEndpoint=Env.apiLiteralGraphQLAPIEndpointOutput,
    ~appSyncAuthenticationType="AWS_IAM",
    ~userFilesS3Bucket=Env.storageLiteralStorageBucketName,
    ~userFilesS3BucketRegion=Env.region,
    (),
  );
