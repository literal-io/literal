module GetAnnotationFragment = [%graphql
  {|
    fragment newTagInputAnnotationFragment on Annotation {
      id
      created
      body {
        ... on TextualBody {
          id
          value
          purpose
          __typename

          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
        }
      }
    }
  |}
];

module PatchAnnotationMutation = [%graphql
  {|
    mutation PatchAnnotation($input: PatchAnnotationInput!) {
      patchAnnotation(input: $input) {
        annotation {
          ...GetAnnotationFragment.NewTagInputAnnotationFragment @bsField(name: "newTagInputAnnotationFragment")
        }
      }
    }
  |}
];
