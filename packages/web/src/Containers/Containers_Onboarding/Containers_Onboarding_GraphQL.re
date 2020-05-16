module CreateHighlightMutation = [%graphql
  {|
    mutation createHighlight($input: CreateHighlightInput!) {
      createHighlight(input: $input) {
        id
        createdAt
        __typename
        ...Containers_NoteEditor_GraphQL.GetHighlightFragment.EditorHighlightFragment @bsField(name: "editorHighlightFragment")
        ...Containers_NoteHeader_GraphQL.GetHighlightFragment.HeaderHighlightFragment @bsField(name: "headerHighlightFragment")
      }
    }
  |}
];
