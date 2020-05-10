exception InvalidState(string);

exception ApolloEmptyCache;
exception ApolloError(ApolloHooksTypes.apolloError);

// TODO: sentry
let report = exn => {
  Js.Console.trace();
  Js.Console.log(exn);
};
