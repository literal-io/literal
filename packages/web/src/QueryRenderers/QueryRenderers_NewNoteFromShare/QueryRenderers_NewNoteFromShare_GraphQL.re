module GetNoteQuery = [%graphql
  {|
    query GetHighlight($creatorUsername: String!, $id: String!) {
      getAnnotation(creatorUsername: $creatorUsername, id: $id) {
        id
        created
        __typename
        ...Containers_NoteEditor_Notes_GraphQL.GetAnnotationFragment.EditorAnnotationFragment @bsField(name: "editorAnnotationFragment")
        ...Containers_NoteHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
      }
    }
  |}
];
