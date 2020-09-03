module GetAnnotationFragment = [%graphql
  {|
    fragment editorNotesAnnotationFragment on Annotation {
      id
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
      target {
        ... on TextualTarget {
          value
          __typename

          textualTargetId: id
          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
        }
        ... on ExternalTarget {
          __typename

          externalTargetId: id
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
