/* Amplify Params - DO NOT EDIT
You can access the following resource attributes as environment variables from your Lambda function
var environment = process.env.ENV
var region = process.env.REGION
var apiLiteralGraphQLAPIIdOutput = process.env.API_LITERAL_GRAPHQLAPIIDOUTPUT
var apiLiteralGraphQLAPIEndpointOutput = process.env.API_LITERAL_GRAPHQLAPIENDPOINTOUTPUT

Amplify Params - DO NOT EDIT */

exports.handler = async (event) => {
    // TODO implement
    const response = {
        statusCode: 200,
        body: JSON.stringify('Hello from Lambda!'),
    };
    return response;
};
