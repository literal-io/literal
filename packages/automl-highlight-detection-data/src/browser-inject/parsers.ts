import { DOMAIN, InjectScope, ParserInterface } from "./types";

export const parsers: { [domain: string]: ParserInterface } = {
  [DOMAIN.WIKIPEDIA]: {
    getUrl: () => "https://en.wikipedia.org/wiki/Special:Random",
    parse: (scope: InjectScope): Text[] => {
      // open all closed sections
      document
        .querySelectorAll(
          "#bodyContent .collapsible-heading,.collapsible-block"
        )
        .forEach((elem) => {
          elem.classList.add("open-block");
        });

      // remove interactive elements
      ($("*") as any).off();

      // remove non-text elements
      document.querySelectorAll("table, h2, img, sup, ol").forEach((elem) => {
        elem.remove();
      });

      // disable all links: replace with spans
      document.querySelectorAll("a, img").forEach((el) => {
        const span = document.createElement("span");
        span.style.color = "#0645ad";
        span.innerHTML = el.innerHTML;
        el.parentNode.replaceChild(span, el);
      });

      const textNodes = scope.getTextNodes(
        document.querySelector("#bodyContent")
      );

      return textNodes;
    },
    getBoundaryAncestorSelector: () => "section",
  },
  [DOMAIN.HACKERNEWS]: {
    // hn uses incremental ids, max id taken on 03/27/20
    getUrl: () =>
      `https://news.ycombinator.com/item?id=${Math.floor(
        Math.random() * 22702482
      )}`,
    parse: (scope: InjectScope): Text[] => {
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });

      const textNodes = Array.from(document.querySelectorAll(".comment"))
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
    getBoundaryAncestorSelector: () => ".comment",
  },
  [DOMAIN.RIBBONFARM]: {
    getUrl: () => {
      const urls = [
        "https://www.ribbonfarm.com/2009/10/07/the-gervais-principle-or-the-office-according-to-the-office/",
        "https://www.ribbonfarm.com/2010/07/26/a-big-little-idea-called-legibility/",
        "https://www.ribbonfarm.com/2015/05/07/weaponized-sacredness/",
        "https://www.ribbonfarm.com/2017/01/05/tendrils-of-mess-in-our-brains/",
        "https://www.ribbonfarm.com/2017/08/17/the-premium-mediocre-life-of-maya-millennial/",
        "https://www.ribbonfarm.com/2020/01/16/the-internet-of-beefs/",
        "https://www.ribbonfarm.com/2018/11/28/the-digital-maginot-line/",
        "https://www.ribbonfarm.com/2017/10/10/the-blockchain-man/",
        "https://www.ribbonfarm.com/2015/09/24/samuel-becketts-guide-to-particles-and-antiparticles/",
        "https://www.ribbonfarm.com/2016/05/12/artem-vs-predator/",
      ];

      return urls[Math.floor(Math.random() * urls.length)];
    },
    parse: (scope: InjectScope): Text[] => {
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });

      document
        .querySelectorAll(".after-post, .post-meta, .sharedaddy, fieldset")
        .forEach((el) => {
          el.remove();
        });

      return scope.getTextNodes(document.querySelector(".entry-content"));
    },
    getBoundaryAncestorSelector: () => "p",
  },
};
