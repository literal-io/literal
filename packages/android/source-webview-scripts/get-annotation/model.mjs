import { xPath } from "./xpath.mjs";

const LANGUAGE = "EN_US";
const TEXT_DIRECTION = getComputedStyle(
  window.document.body
).direction.toUpperCase();

export const makeXPathSelectorFromRange = ({
  container,
  startPosition,
  endPosition,
}) => ({
  type: "XPATH_SELECTOR",
  value: xPath(container, true),
  refinedBy: [
    {
      type: "TEXT_POSITION_SELECTOR",
      start: startPosition,
      end: endPosition,
    },
  ],
});

export const makeAnnotationFromSelection = ({ selection }) => {
  const range = selection.getRangeAt(0);
  return {
    context: ["http://www.w3.org/ns/anno.jsonld"],
    motivation: ["HIGHLIGHTING"],
    type: "ANNOTATION",
    target: [
      {
        value: selection.toString(),
        format: "TEXT_PLAIN",
        language: LANGUAGE,
        processingLanguage: LANGUAGE,
        textDirection: TEXT_DIRECTION,
      },
      {
        source: {
          id: window.location.href,
          format: "TEXT_HTML",
          language: LANGUAGE,
          processingLanguage: LANGUAGE,
          textDirection: TEXT_DIRECTION,
          type: "TEXT",
        },
        selector: [
          {
            type: "RANGE_SELECTOR",
            startSelector: makeXPathSelectorFromRange({
              container: range.startContainer,
              startPosition: range.startOffset,
              endPosition: range.startContainer.textContent.length,
            }),
            endSelector: makeXPathSelectorFromRange({
              container: range.endContainer,
              startPosition: 0,
              endPosition: range.endOffset,
            }),
          },
        ],
      },
    ],
  };
};
