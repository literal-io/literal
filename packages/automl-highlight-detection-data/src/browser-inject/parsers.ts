import { DOMAIN, InjectScope } from "./types";

export const parsers = {
  [DOMAIN.WIKIPEDIA]: (scope: InjectScope): Text[] => {
    // open all closed sections
    document
      .querySelectorAll("#bodyContent .collapsible-heading,.collapsible-block")
      .forEach(elem => {
        elem.classList.add("open-block");
      });

    // remove interactive elements
    ($("*") as any).off();

    // remove non-text elements
    document.querySelectorAll("table, h2, img, sup, ol").forEach(elem => {
      elem.remove();
    });

    // disable all links: replace with spans
    document.querySelectorAll("a, img").forEach(el => {
      const span = document.createElement("span");
      span.style.color = "#0645ad";
      span.innerHTML = el.innerHTML;
      el.parentNode.replaceChild(span, el);
    });

    const textNodes = scope.getTextNodes(
      document.querySelector("#bodyContent")
    );

    return textNodes;
  }
};
