exception InvalidState(string);

exception ApolloEmptyCache;
exception ApolloEmptyData;

exception ApolloError(ApolloHooksTypes.apolloError);
exception GraphQLError(ApolloHooksTypes.graphqlError)

exception DeccoDecodeError(Decco.decodeError);

exception AuthenticationRequired;

// TODO: sentry
let report = exn => {
  Js.Console.trace();
  Js.Console.log(exn);
};
