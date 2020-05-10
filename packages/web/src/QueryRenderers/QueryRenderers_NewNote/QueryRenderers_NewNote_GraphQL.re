module GetNoteQuery = [%graphql
  {|
    query GetHighlight($id: ID!) {
      getHighlight(id: $id) {
        id
        text
        note
        ...Containers_NewNoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
        ...Containers_NewNoteEditor_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
      }
    }
  |}
];
