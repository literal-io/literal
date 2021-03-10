module AnnotationFragment = [%graphql
  {|
    fragment externalTargetMetadataAnnotationFragment on Annotation {
      id
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
  |}
];

module Webview = {
  let makeExternalTarget = externalTarget =>
    Lib_WebView_Model_Target.(
      ExternalTarget(
        makeExternalTarget(
          ~id=externalTarget##externalTargetId,
          ~language=?
            externalTarget##language
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~processingLanguage=?
            externalTarget##processingLanguage
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~accessibility=?externalTarget##accessibility,
          ~rights=?externalTarget##rights,
          ~textDirection=?
            externalTarget##textDirection
            ->Belt.Option.map(Lib_GraphQL_TextDirection.toString),
          ~format=?
            externalTarget##format
            ->Belt.Option.map(Lib_GraphQL_Format.toString),
          ~type_=?
            externalTarget##type_
            ->Belt.Option.map(Lib_GraphQL_ResourceType.toJs),
          (),
        ),
      )
    );

  let makeTextualTarget = textualTarget =>
    Lib_WebView_Model_Target.(
      TextualTarget(
        makeTextualTarget(
          ~id=textualTarget##textualTargetId,
          ~value=textualTarget##value,
          ~language=?
            textualTarget##language
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~processingLanguage=?
            textualTarget##processingLanguage
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~accessibility=?textualTarget##accessibility,
          ~rights=?textualTarget##rights,
          ~textDirection=?
            textualTarget##textDirection
            ->Belt.Option.map(Lib_GraphQL_TextDirection.toString),
          ~format=?
            textualTarget##format
            ->Belt.Option.map(Lib_GraphQL_Format.toString),
          (),
        ),
      )
    );

  let makeTextualBody = textualBody => {
    Lib_WebView_Model_Body.(
      TextualBody(
        makeTextualBody(
          ~language=?
            textualBody##language
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~processingLanguage=?
            textualBody##processingLanguage
            ->Belt.Option.map(Lib_GraphQL_Language.toString),
          ~accessibility=?textualBody##accessibility,
          ~rights=?textualBody##rights,
          ~textDirection=?
            textualBody##textDirection
            ->Belt.Option.map(Lib_GraphQL_TextDirection.toString),
          ~format=?
            textualBody##format->Belt.Option.map(Lib_GraphQL_Format.toString),
          ~purpose=?
            textualBody##purpose
            ->Belt.Option.map(d =>
                d->Belt.Array.map(Lib_GraphQL_Motivation.toString)
              ),
          ~value=textualBody##value,
          ~id=textualBody##id,
          (),
        ),
      )
    );
  };

  let makeRangeSelector = selector => {
    let makeTextPositionSelector = s =>
      switch (s) {
      | `TextPositionSelector(textPositionSelector) =>
        Some(
          Lib_WebView_Model_Selector.(
            TextPositionSelector(
              makeTextPositionSelector(
                ~start=textPositionSelector##start,
                ~end_=textPositionSelector##end_,
                ~type_="TEXT_POSITION_SELECTOR",
                (),
              ),
            )
          ),
        )
      | `Nonexhaustive => None
      };

    let makeXPathSelector = s =>
      switch (s) {
      | `XPathSelector(xPathSelector) =>
        Some(
          Lib_WebView_Model_Selector.XPathSelector(
            Lib_WebView_Model_Selector.makeXPathSelector(
              ~value=xPathSelector##value,
              ~refinedBy=?
                xPathSelector##refinedBy
                ->Belt.Option.map(a =>
                    a->Belt.Array.keepMap(makeTextPositionSelector)
                  ),
              ~type_="XPATH_SELECTOR",
              (),
            ),
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
          Lib_WebView_Model_Selector.(
            RangeSelector(
              makeRangeSelector(
                ~startSelector,
                ~endSelector,
                ~type_="RANGE_SELECTOR",
                (),
              ),
            )
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
        Lib_WebView_Model_Target.(
          SpecificTarget(
            makeSpecificTarget(
              ~id=specificTarget##specificTargetId,
              ~source=externalTarget,
              ~selector,
              (),
            ),
          )
        ),
      )
    | _ => None
    };
  };

  let makeTarget = target =>
    switch (target) {
    | `ExternalTarget(t) => t->makeExternalTarget->Js.Option.some
    | `SpecificTarget(t) => t->makeSpecificTarget
    | `TextualTarget(t) => t->makeTextualTarget->Js.Option.some
    | `Nonexhaustive => None
    };

  let makeBody = body =>
    switch (body) {
    | `TextualBody(t) => t->makeTextualBody->Js.Option.some
    | `Nonexhaustive => None
    };

  let makeAnnotation = annotation =>
    Lib_WebView_Model_Annotation.make(
      ~id=annotation##id,
      ~target=annotation##target->Belt.Array.keepMap(makeTarget),
      ~body=?
        annotation##body
        ->Belt.Option.map(a => a->Belt.Array.keepMap(makeBody)),
      (),
    );
};
