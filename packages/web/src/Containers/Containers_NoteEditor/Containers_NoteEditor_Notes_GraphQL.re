module GetAnnotationFragment = [%graphql
  {|
    fragment editorAnnotationFragment on Annotation {
      id
      body {
        ... on TextualBody {
          id
          value
          purpose
        }
      }
      target {
        ... on TextualTarget {
          value
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
        id
      }
    }
  }
|}
];
