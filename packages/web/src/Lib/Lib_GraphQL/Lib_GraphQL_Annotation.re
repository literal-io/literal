let cacheAnnotationFragment =
  ApolloClient.gql(.
    {|
  fragment CacheAnnotation on Annotation {
    id
    created
    modified
    body {
      ... on TextualBody {
        id
        value
        purpose
        __typename

        format
        language
        processingLanguage
        textDirection
        accessibility
        rights
      }
    }
    target {
      ... on TextualTarget {
        value
        __typename

        textualTargetId: id
        format
        language
        processingLanguage
        textDirection
        accessibility
        rights
      }
      ... on ExternalTarget {
        externalTargetId: id
        language
        processingLanguage
        textDirection
        format
        accessibility
        rights
        type_: type
        __typename
      }
      ... on SpecificTarget {
        __typename
        specificTargetId: id
        source {
          ... on ExternalTarget {
            externalTargetId: id
            language
            processingLanguage
            textDirection
            format
            accessibility
            rights
            type_: type
            __typename
          }
        }
        selector {
          ... on RangeSelector {
            __typename
            startSelector {
              ... on XPathSelector {
                __typename
                value
                refinedBy {
                  ... on TextPositionSelector {
                    __typename
                    start
                    end_: end
                  }
                }
              }
            }
            endSelector {
              ... on XPathSelector {
                __typename
                value
                refinedBy {
                  ... on TextPositionSelector {
                    __typename
                    start
                    end_: end
                  }
                }
              }
            }
          }
        }
      }
    }
  }
|},
  );

let readCache = id => {
  Apollo.Client.readFragment(
    Providers_Apollo.client,
    {id: "Annotation:" ++ id, fragment: cacheAnnotationFragment},
  );
};
