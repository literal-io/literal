module GetAnnotationCollection = {
  module Query = [%graphql
    {|
      query getAnnotationCollection($creatorUsername: String!, $id: String!) {
        getAnnotationCollection(creatorUsername: $creatorUsername, id: $id) {
          label
          first {
            items(limit: 100) {
              items {
                annotation {
                  created
                  id
                  ...Containers_NoteEditor_Notes_GraphQL.GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
                  ...Containers_NoteHeader_GraphQL.GetAnnotationFragment.HeaderAnnotationFragment @bsField(name: "headerAnnotationFragment")
                }
              }
            }
          }
        }
      }
    |}
  ];
};
