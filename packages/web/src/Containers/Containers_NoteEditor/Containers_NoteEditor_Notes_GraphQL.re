module GetAnnotationFragment = [%graphql
  {|
    fragment editorAnnotationFragment on Annotation {
      id
      body {
        ... on TextualBody {
          id
          value
          purpose

          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
        }
      }
      target {
        ... on TextualTarget {
          value
          __typename

          id
          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
        }
        ... on ExternalTarget {
          __typename

          id
          format
          language
          processingLanguage
          textDirection
          type_: type
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
        id
      }
    }
  }
|}
];
