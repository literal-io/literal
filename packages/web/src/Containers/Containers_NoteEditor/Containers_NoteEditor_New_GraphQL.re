module CreateAnnotationMutation = [%graphql
  {|
    mutation CreateAnnotation($input: CreateAnnotationInput!) {
      createAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];
