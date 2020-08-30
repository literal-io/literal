module PatchAnnotationMutation = [%graphql
  {|
    mutation PatchAnnotation(
      $input: PatchAnnotationInput!
    ) {
      patchAnnotation(input: $input) {
        annotation {
          id
          ...Containers_NoteEditor_Notes_GraphQL.GetAnnotationFragment.EditorAnnotationFragment @bsField(name: "editorAnnotationFragment")
          ...Containers_NoteHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
        }
      }
    }
  |}
];

module GetAnnotationFragment = [%graphql
  {|
    fragment editorAnnotationFragment on Annotation {
      id
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
    }
  |}
];
