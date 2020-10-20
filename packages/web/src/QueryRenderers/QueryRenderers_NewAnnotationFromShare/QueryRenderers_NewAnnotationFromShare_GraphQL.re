module GetAnnotationQuery = [%graphql
  {|
    query GetAnnotation($creatorUsername: String!, $id: String!) {
      getAnnotation(creatorUsername: $creatorUsername, id: $id) {
        id
        created
        __typename
        ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        ...Containers_NewAnnotationFromShareHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
      }
    }
  |}
];

module CreateAnnotationFromExternalTargetMutation = [%graphql
  {|
    mutation CreateAnnotationFromExternalTarget($input: CreateAnnotationFromExternalTargetInput!) {
      createAnnotationFromExternalTarget(input: $input) {
        id
        created
        __typename
        ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        ...Containers_NewAnnotationFromShareHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
      }
    }
  |}
]
