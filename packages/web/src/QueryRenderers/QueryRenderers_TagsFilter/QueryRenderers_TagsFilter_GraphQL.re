module AnnotationCollectionLabelAutocomplete = [%graphql
  {|
    query annotationCollectionLabelAutocomplete(
      $input: AnnotationCollectionLabelAutocompleteInput!
    ) {
      annotationCollectionLabelAutocomplete(
        input: $input
      ) {
        items {
          id
          label
        }
      }
    }
  |}
];
