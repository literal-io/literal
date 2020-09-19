module GetAnnotationCollectionFragment = [%graphql
  {|
    fragment AnnotationCollectionHeader_AnnotationCollection on AnnotationCollection {
      label
    }
  |}
];

module GetAnnotationFragment = [%graphql
  {|
    fragment AnnotationCollectionHeader_Annotation on Annotation {
      id
      body {
        ... on TextualBody {
          __typename
          id
          value
          purpose
        }
      }
    }
  |}
];

module DeleteAnnotationMutation = [%graphql
  {|
    mutation DeleteAnnotation($input: DeleteAnnotationInput!) {
      deleteAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];
