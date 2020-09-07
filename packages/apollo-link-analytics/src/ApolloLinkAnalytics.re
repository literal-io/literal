let make = onOperation =>
  ApolloLink.make((operation, forward) => {
    forward(operation)
    ->ApolloLink.Observable.map(data => {
        onOperation(operation, data);
        data;
      })
    ->Js.Option.some
  });
