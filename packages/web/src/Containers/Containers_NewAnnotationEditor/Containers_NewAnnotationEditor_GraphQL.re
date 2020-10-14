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

module CreateAnnotationFromExternalTargetMutation = [%graphql
  {|
    mutation CreateAnnotationFromExternalTarget($input: CreateAnnotationFromExternalTargetInput!) {
      createAnnotationFromExternalTarget(input: $input) {
        id
        ...Containers_NewAnnotationFromShareEditor_GraphQL.GetAnnotationFragment.EditorNewFromShareAnnotationFragment @bsField(name: "editorNewFromShareAnnotationFragment")
        ...Containers_NewAnnotationFromShareHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerNewFromShareAnnotationFragment")
      }
    }
  |}
];
