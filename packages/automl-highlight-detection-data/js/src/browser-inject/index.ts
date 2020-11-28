import * as R from "ramda";
import {
  DOMAIN,
  SerializedScope,
  RehydratedScope,
  SelectionAnnotation,
} from "./types";
import { parsers } from "./parsers";
import { scope } from "./scope";

export * from "./types";
export * from "./parsers";

/**
 * NOTE: We have to be careful around the logic in the injected fn, as the
 * function is serialized into the browser context, **everything** it
 * accesses must be an explicit arg, and not just closed over. Typescript
 * compilation can disrupt this subtley.
 */
export const browserInject = (
  domain: DOMAIN,
  execute: any
): Promise<{
  annotations: SelectionAnnotation[];
  size: {
    width: number;
    height: number;
    scale: number;
  };
  text: string;
}> => {
  const serializedScope: SerializedScope = R.map((fn) => fn.toString(), {
    ...scope,
    ...parsers[domain],
  });

  return execute((stringifiedArgs: string) => {
    const serializedScope = JSON.parse(stringifiedArgs);
    const scope = Object.keys(serializedScope).reduce(
      (memo, key) => ({
        ...memo,
        [key]: eval(serializedScope[key]),
      }),
      {}
    ) as RehydratedScope;

    const textNodes = scope.parse(scope);
    if (!textNodes || textNodes.length === 0) {
      return {
        annotations: [],
        text: "",
        size: scope.getViewportSize(),
      };
    }

    const range = scope.getRandomRange(
      textNodes,
      scope.getBoundaryAncestorSelector(),
      scope.getViewportSize()
    );
    if (!range) {
      return {
        annotations: [],
        text: "",
        size: scope.getViewportSize(),
      };
    }

    scope.scrollToRange(
      range,
      scope.getViewportSize(),
      scope.getScrollOffsetHeight ? scope.getScrollOffsetHeight() : 0
    );
    scope.styleRange(range, scope.getBoundaryAncestorSelector());

    const annotations = scope.getSelectionAnnotations(
      range,
      scope.getViewportSize()
    );
    return {
      annotations,
      text: window.getSelection().toString(),
      size: scope.getViewportSize(),
    };
  }, JSON.stringify(serializedScope));
};
