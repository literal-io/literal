module Client = {
  type readFragmentArgs('a) = {
    fragment: ReasonApolloTypes.queryString,
    id: string,
  };

  [@bs.send] [@bs.return nullable]
  external readFragment:
    (ApolloClient.generatedApolloClient, readFragmentArgs('a)) =>
    option(Js.Json.t) =
    "readFragment";
};
