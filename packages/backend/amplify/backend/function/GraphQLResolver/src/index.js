/* Amplify Params - DO NOT EDIT
You can access the following resource attributes as environment variables from your Lambda function
var environment = process.env.ENV
var region = process.env.REGION
var authLiteralAuthUserPoolId = process.env.AUTH_LITERALAUTH_USERPOOLID
var apiLiteralGraphQLAPIIdOutput = process.env.API_LITERAL_GRAPHQLAPIIDOUTPUT
var apiLiteralGraphQLAPIEndpointOutput = process.env.API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT
var storageLiteralStorageBucketName = process.env.STORAGE_LITERALSTORAGE_BUCKETNAME

Amplify Params - DO NOT EDIT */

exports.handler = async (event) => {
    // TODO implement
    const response = {
        statusCode: 200,
        body: JSON.stringify('Hello from Lambda!'),
    };
    return response;
};
