module Env = {
  [@bs.val] external env: string = "process.env.ENV";
  [@bs.val] external region: string = "process.env.REGION";
  [@bs.val]
  external apiLiteralGraphQLAPIEndpointOutput: string =
    "process.env.API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT";
  [@bs.val]
  external storageLiteralStorageBucketName: string =
    "process.env.STORAGE_LITERALSTORAGE_BUCKETNAME";
  [@bs.val]
  external authLiteralAuthUserPoolId: string =
    "process.env.AUTH_LITERALAUTH_USERPOOLID";
  [@bs.val]
  external authLiteralAuthUserPooWebClientId: string =
    "process.env.AUTH_LITERALAUTH_USERPOOLWEBCLIENTID";
};

let apiOrigin = Env.env === "production"
  ? "https://literal.io"
  : "https://staging.literal.io"
