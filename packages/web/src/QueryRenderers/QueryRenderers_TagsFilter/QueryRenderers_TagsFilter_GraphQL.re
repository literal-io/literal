module AnnotationCollectionLabelAutocomplete = [%graphql
  {|
    query annotationCollectionLabelAutocomplete(
      $creatorUsername: String!,
      $labelBeginsWith: String!
    ) {
      listAnnotationCollectionsByLabel(
        creatorUsername: $creatorUsername,
        label: {
          beginsWith: $labelBeginsWith
        }
      ) {
        items {
          id
          label
        }
      }
    }
  |}
];
