module FilterAnnotationCollections = [%graphql
  {|
    query FilterAnnotationCollections($input: String!) {
      listAnnotationCollections(filter: {
        label: {
          beginsWith: $input
        }
      }) {
        items {
          id
          label
        }
      }
    }
  |}
];
