module PatchAnnotationMutation = [%graphql
  {|
    mutation PatchAnnotation(
      $input: PatchAnnotationInput!
    ) {
      patchAnnotation(input: $input) {
        annotation {
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
    }
  |}
];

module GetAnnotationFragment = [%graphql
  {|
    fragment editorNewFromShareAnnotationFragment on Annotation {
      id
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
