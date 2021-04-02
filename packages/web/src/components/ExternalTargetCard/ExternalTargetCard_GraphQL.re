module ExternalTargetFragment = [%graphql
  {|
    fragment externalTargetCard_ExternalTargetFragment on ExternalTarget {
      externalTargetId: id
      format
      __typename
    }
  |}
];

module TextualTargetFragment = [%graphql
  {|
    fragment externalTargetCard_TextualTargetFragment on TextualTarget {
      textualTargetId: id
      value
      __typename
    }
  |}
];
