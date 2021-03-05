module GetAnnotationFragment = [%graphql
  {|
    fragment editorNotesAnnotationFragment on Annotation {
      id
      created
      modified
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
      }
      ...ExternalTargetMetadata_GraphQL.AnnotationFragment.ExternalTargetMetadataAnnotationFragment @bsField(name: "externalTargetMetadataAnnotationFragment")
    }
  |}
];

module PatchAnnotationMutation = [%graphql
  {|
    mutation PatchAnnotation($input: PatchAnnotationInput!) {
      patchAnnotation(input: $input) {
        annotation {
          ...GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
    }
  |}
];
