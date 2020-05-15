exception InvalidState(string);

exception ApolloEmptyCache;
exception ApolloError(ApolloHooksTypes.apolloError);

exception DeccoDecodeError(Decco.decodeError);

// TODO: sentry
let report = exn => {
  Js.Console.trace();
  Js.Console.log(exn);
};
