module GetAnnotationFragment = [%graphql
  {|
    fragment editorNotesAnnotationFragment on Annotation {
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
          ...ExternalTargetCard_GraphQL.TextualTargetFragment.ExternalTargetCard_TextualTargetFragment @bsField(name: "externalTargetCard_TextualTargetFragment")
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
          ...ExternalTargetCard_GraphQL.ExternalTargetFragment.ExternalTargetCard_ExternalTargetFragment @bsField(name: "externalTargetCard_ExternalTargetFragment")
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
              ...ExternalTargetCard_GraphQL.ExternalTargetFragment.ExternalTargetCard_ExternalTargetFragment @bsField(name: "externalTargetCard_ExternalTargetFragment")
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
          state {
            ... on TimeState {
              __typename
              cached
              sourceDate
            }
          }
        }
      }
    }
  |}
];

module PatchAnnotationMutation = [%graphql
  {|
    mutation PatchAnnotation($input: PatchAnnotationInput!) {
      patchAnnotation(input: $input) {
        annotation {
          ...GetAnnotationFragment.EditorNotesAnnotationFragment @bsField(name: "editorAnnotationFragment")
        }
      }
    }
  |}
];

module Webview = {
  let makeState =
    fun
    | `TimeState(t) =>
      t->LiteralModel.State.makeTimeStateFromGraphQL->Js.Option.some
    | `Nonexhaustive => None;

  let makeSpecificTargetSelector = {
    let makeXPathSelector =
      fun
      | `XPathSelector(xPathSelector) =>
        xPathSelector
        |> LiteralModel.Selector.makeXPathSelectorFromGraphQL(
             ~makeRefinedBy={
               fun
               | `TextPositionSelector(textPositionSelector) =>
                 textPositionSelector
                 ->LiteralModel.Selector.makeTextPositionSelectorFromGraphQL
                 ->Js.Option.some
               | `Nonexhaustive => None;
             },
           )
        |> Js.Option.some
      | `Nonexhaustive => None;

    fun
    | `RangeSelector(rangeSelector) =>
      LiteralModel.Selector.makeRangeSelectorFromGraphQL(
        ~makeStartSelector=makeXPathSelector,
        ~makeEndSelector=makeXPathSelector,
        rangeSelector,
      )
    | `Nonexhaustive => None;
  };

  let makeAnnotation = annotation =>
    LiteralModel.Annotation.makeAnnotationFromGraphQL(
      ~makeTarget={
        fun
        | `ExternalTarget(t) =>
          t
          ->LiteralModel.Target.makeExternalTargetFromGraphQL
          ->Js.Option.some
        | `SpecificTarget(t) =>
          LiteralModel.Target.makeSpecificTargetFromGraphQL(
            ~makeSelector=makeSpecificTargetSelector,
            ~makeState,
            t,
          )
        | `TextualTarget(t) =>
          t
          ->LiteralModel.Target.makeTextualTargetFromGraphQL
          ->Js.Option.some
        | `Nonexhaustive => None;
      },
      ~makeBody={
        fun
        | `TextualBody(t) =>
          t->LiteralModel.Body.makeTextualBodyFromGraphQL->Js.Option.some
        | `Nonexhaustive => None;
      },
      annotation,
    );
};
