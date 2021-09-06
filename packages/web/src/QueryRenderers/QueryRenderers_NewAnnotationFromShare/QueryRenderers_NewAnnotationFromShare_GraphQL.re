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

module CreateAnnotationMutation = [%graphql
  {|
    mutation NewAnnotationFromShare_CreateAnnotation($input: CreateAnnotationInput!) {
      createAnnotation(input: $input) {
        annotation {
          id
          created
          __typename
          ...Containers_AnnotationEditor_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
          ...Containers_NewAnnotationFromShareHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
        }
      }
    }
  |}
];
