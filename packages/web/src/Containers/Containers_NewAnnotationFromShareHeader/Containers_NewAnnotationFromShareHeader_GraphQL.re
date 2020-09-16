module GetAnnotationFragment = [%graphql
  {|
    fragment headerAnnotationFragment on Annotation {
      id
    }
  |}
];

module DeleteAnnotationMutation = [%graphql
  {|
    mutation deleteAnnotation($input: DeleteAnnotationInput!) {
      deleteAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];

