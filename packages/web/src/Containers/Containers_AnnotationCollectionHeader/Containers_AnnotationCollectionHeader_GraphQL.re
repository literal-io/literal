module GetAnnotationCollectionFragment = [%graphql
  {|
    fragment AnnotationCollectionHeader_AnnotationCollection on AnnotationCollection {
      label
      id
      type_: type
    }
  |}
];

let cacheAnnotationCollectionFragment =
  ApolloClient.gql(.
    {|
  fragment Cache_AnnotationCollectionHeader_AnnotationCollection on AnnotationCollection {
    label
    id
    type_: type
  }
|},
  );

module GetAnnotationFragment = [%graphql
  {|
    fragment AnnotationCollectionHeader_Annotation on Annotation {
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
        ... on TextualTarget {
          value
          __typename

          textualTargetId: id
        }
      }
    }
  |}
];

module DeleteAnnotationMutation = [%graphql
  {|
    mutation DeleteAnnotation($input: DeleteAnnotationInput!) {
      deleteAnnotation(input: $input) {
        annotation {
          id
        }
      }
    }
  |}
];
