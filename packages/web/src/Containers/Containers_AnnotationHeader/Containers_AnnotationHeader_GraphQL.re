module GetAnnotationFragment = [%graphql
  {|
    fragment headerAnnotationFragment on Annotation {
      id
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
