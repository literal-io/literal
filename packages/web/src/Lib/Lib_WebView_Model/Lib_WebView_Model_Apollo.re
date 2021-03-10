let unsafeAsCache = [%raw
  {|
  function asCache(annotation) {
    const selectorAsCache = (selector) => {
      if (selector.__typename === "RangeSelector") {
        return {
          ...selector,
          startSelector: selectorAsCache(selector.startSelector),
          endSelector: selectorAsCache(selector.endSelector),
          type_: selector.type
        }
      } else if (selector.__typename === "XPathSelector") {
        return {
          ...selector,
          refinedBy: selector.refinedBy
            ? selector.refinedBy.map(selectorAsCache)
            : null,
          type_: selector.type
        }
      } else if (selector.__typename === "TextPositionSelector") {
        return {
          ...selector,
          end_: selector.end,
          type_: selector.type
        }
      }
    }

    const targetAsCache = (target) => {
      if (target.__typename === "SpecificTarget") {
        return {
          ...target,
          specificTargetId: target.id,
          selector: target.selector.map(selectorAsCache),
          source: targetAsCache(target.source)
        }
      } else if (target.__typename === "ExternalTarget") {
        return {
          ...target,
          externalTargetId: target.id,
          type_: target.type
        }
      } else if (target.__typename === "TextualTarget") {
        return {
          ...target,
          textualTargetId: target.id
        }
      }
      return target
    }

    return {
      ...annotation,
      target:
        annotation.target
          ? annotation.target.map(targetAsCache)
          : null
    }
  }
|}
];

let writeToCache = (~annotation, ~currentUser) => {
  let cacheAnnotation =
    annotation->Lib_WebView_Model_Annotation.encode->unsafeAsCache;

  annotation.body
  ->Belt.Option.getWithDefault([||])
  ->Belt.Array.keepMap(body =>
      switch (body) {
      | Lib_WebView_Model_Body.TextualBody(textualBody)
          when
            textualBody.purpose
            ->Belt.Option.map(a =>
                a->Belt.Array.some(purpose => purpose == "TAGGING")
              )
            ->Belt.Option.getWithDefault(false) =>
        textualBody.id
      | _ => None
      }
    )
  ->Belt.Array.forEach(annotationCollectionId => {
      Js.log3("writeToCache", cacheAnnotation, annotationCollectionId);
      Lib_GraphQL_AnnotationCollection.Apollo.setAnnotationInCollection(
        ~annotation=cacheAnnotation,
        ~currentUser,
        ~annotationCollectionId,
      );
    });
};
