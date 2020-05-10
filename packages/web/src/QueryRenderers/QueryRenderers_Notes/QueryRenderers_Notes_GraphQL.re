module ListHighlightsQuery = [%graphql
  {|
    query ListHighlights {
      listHighlights(limit: 100) {
        items {
          id
          createdAt
          ...Containers_NoteEditor_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
          ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
        }
      }
    }
  |}
];
