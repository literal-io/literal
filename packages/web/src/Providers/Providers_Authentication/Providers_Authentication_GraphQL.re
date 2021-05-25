module MergeUserIdentitiesMutation = [%graphql
  {|
      mutation MergeUserIdentities($input: MergeUserIdentitiesInput!) {
        mergeUserIdentities(input: $input) {
          _
        }
      }
  |}
];

module Input = {
  let make =
      (
        ~sourceGuestUserCredentials: Providers_Authentication_Credentials.awsCredentials,
        ~targetUserCredentials: AmazonCognitoIdentity.UserSession.t,
      ) => {
    "targetUserIdToken":
      AmazonCognitoIdentity.(
        targetUserCredentials
        ->UserSession.getIdToken
        ->IdToken.getJwtToken
        ->JwtToken.toString
      ),
  };
};
