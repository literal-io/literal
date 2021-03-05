module AnnotationFragment = [%graphql
  {|
    fragment externalTargetMetadataAnnotationFragment on Annotation {
      target {
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
  |}
];

module Webview = {
  let makeExternalTarget = externalTarget =>
    Lib_WebView_Model.Annotation.Target.(
      ExternalTarget({
        id: externalTarget##externalTargetId,
        language:
          externalTarget##language
          ->Belt.Option.map(Lib_GraphQL_Language.toString),
        processingLanguage:
          externalTarget##processingLanguage
          ->Belt.Option.map(Lib_GraphQL_Language.toString),
        accessibility: externalTarget##accessibility,
        rights: externalTarget##rights,
        textDirection:
          externalTarget##textDirection
          ->Belt.Option.map(Lib_GraphQL_TextDirection.toString),
        format:
          externalTarget##format
          ->Belt.Option.map(Lib_GraphQL_Format.toString),
        type_:
          externalTarget##type_
          ->Belt.Option.map(Lib_GraphQL_ResourceType.toJs),
      })
    );

  let makeRangeSelector = selector => {
    let makeTextPositionSelector = s =>
      switch (s) {
      | `TextPositionSelector(textPositionSelector) =>
        Some(
          Lib_WebView_Model.Annotation.Selector.(
            TextPositionSelector({
              start: textPositionSelector##start,
              end_: textPositionSelector##end_,
              type_: "TEXT_POSITION_SELECTOR",
            })
          ),
        )
      | `Nonexhaustive => None
      };

    let makeXPathSelector = s =>
      switch (s) {
      | `XPathSelector(xPathSelector) =>
        Some(
          Lib_WebView_Model.Annotation.Selector.(
            XPathSelector({
              value: xPathSelector##value,
              refinedBy:
                xPathSelector##refinedBy
                ->Belt.Option.map(a =>
                    a->Belt.Array.keepMap(makeTextPositionSelector)
                  ),
              type_: "XPATH_SELECTOR",
            })
          ),
        )
      | `Nonexhaustive => None
      };

    switch (selector) {
    | `RangeSelector(rangeSelector) =>
      switch (
        makeXPathSelector(rangeSelector##startSelector),
        makeXPathSelector(rangeSelector##endSelector),
      ) {
      | (Some(startSelector), Some(endSelector)) =>
        Some(
          Lib_WebView_Model.Annotation.Selector.(
            RangeSelector({
              startSelector,
              endSelector,
              type_: "RANGE_SELECTOR",
            })
          ),
        )
      | _ => None
      }
    | `Nonexhaustive => None
    };
  };

  let makeSpecificTarget = specificTarget => {
    let selector =
      specificTarget##selector->Belt.Array.keepMap(makeRangeSelector);

    let externalTarget =
      switch (specificTarget##source) {
      | `ExternalTarget(target) => Some(makeExternalTarget(target))
      | `Nonexhaustive => None
      };

    switch (externalTarget) {
    | Some(externalTarget) when Js.Array2.length(selector) > 0 =>
      Some(
        Lib_WebView_Model.Annotation.Target.(
          SpecificTarget({
            id: specificTarget##specificTargetId,
            source: externalTarget,
            selector,
          })
        ),
      )
    | _ => None
    };
  };

  let makeTarget = target =>
    switch (target) {
    | `ExternalTarget(t) => t->makeExternalTarget->Js.Option.some
    | `SpecificTarget(t) => t->makeSpecificTarget
    | `Nonexhaustive => None
    };
};
