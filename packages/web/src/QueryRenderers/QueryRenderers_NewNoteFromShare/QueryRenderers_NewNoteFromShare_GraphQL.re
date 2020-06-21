module GetNoteQuery = [%graphql
  {|
    query GetHighlight($id: ID!) {
      getHighlight(id: $id) {
        id
        text
        note
        ...Containers_NewNoteFromShareHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
        ...Containers_NoteEditor_NewFromShare_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
      }
    }
  |}
];
