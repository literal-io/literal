module FilterTags = [%graphql
  {|
    query filterTags($input: String!) {
      listTags(filter: {
        text: {
          beginsWith: $input
        }
      }) {
        items {
          text
          id
        }
      }
    }
  |}
];
