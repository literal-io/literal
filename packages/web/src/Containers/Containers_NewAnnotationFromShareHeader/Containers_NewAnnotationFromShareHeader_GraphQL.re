module GetAnnotationFragment = [%graphql
  {|
    fragment headerAnnotationFragment on Annotation {
      id
      body {
        ... on TextualBody {
          __typename
          id
          value
          purpose
        }
      }
      target {
        ... on ExternalTarget {
          __typename
          externalTargetId: id
          format
        }
        ... on SpecificTarget {
          __typename
          specificTargetId: id
          source {
            ... on ExternalTarget {
              externalTargetId: id,
              format
              __typename
            }
          }
        }
      }
    }
  |}
];

module DeleteAnnotationMutation = [%graphql
  {|
    mutation deleteAnnotation($input: DeleteAnnotationInput!) {
      deleteAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];

