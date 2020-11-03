import { DOMAIN, InjectScope, ParserInterface } from "./types";

export const parsers: { [domain: string]: ParserInterface } = {
  [DOMAIN.WIKIPEDIA]: {
    getUrl: () => {
      const urls = [
        "https://web.archive.org/web/20200913110219/https://en.wikipedia.org/wiki/Cat_gap",
        "https://web.archive.org/web/20200816082257/https://en.wikipedia.org/wiki/Fossil_record",
        "https://web.archive.org/web/20161124065336/https://en.wikipedia.org/wiki/Exoskeletons",
        "https://web.archive.org/web/20161111063316/https://en.wikipedia.org/wiki/Human_skeleton",
        "https://web.archive.org/web/20161025050725/https://en.wikipedia.org/wiki/Human_iron_metabolism",
        "https://web.archive.org/web/20161020233813/https://en.wikipedia.org/wiki/Mammals",
        "https://web.archive.org/web/20161012192447/https://en.wikipedia.org/wiki/Organism",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Life",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Biological_process",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Bacteria",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Plant",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Science",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Synthetic_biology",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Stimulus_(physiology)",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Gaia_hypothesis",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Abiogenesis",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Fungi",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Earth",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Biological_organization",
        "https://web.archive.org/web/20161012143432/https://en.wikipedia.org/wiki/Physics",
      ];
      return urls[Math.floor(Math.random() * urls.length)];
    },
    isUrlEqual: (url1: string, url2: string): boolean => {
      const getPath = (url: string) => {
        const archiveRegex = /https:\/\/web\.archive\.org\/web\/.*?\/(.*)/;
        const match = archiveRegex.exec(url);
        return match && match.length === 2 ? match[1] : null;
      };
      const path1 = getPath(url1);
      const path2 = getPath(url2);

      return path1 && path2 && path1 === path2;
    },
    parse: (scope: InjectScope): Text[] => {
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove()
      }

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
    getUrl: () => {
      const urls = [
        "https://web.archive.org/web/20201014022648/https://news.ycombinator.com/item?id=24770617",
        "https://web.archive.org/web/20201014022752/https://news.ycombinator.com/item?id=24770424",
        "https://web.archive.org/web/20201014023549/https://news.ycombinator.com/item?id=24771623",
        "https://web.archive.org/web/20201014022706/https://news.ycombinator.com/item?id=24766682",
        "https://web.archive.org/web/20201014022724/https://news.ycombinator.com/item?id=24767378",
        "https://web.archive.org/web/20201014022653/https://news.ycombinator.com/item?id=24765798",
        "https://web.archive.org/web/20201014022659/https://news.ycombinator.com/item?id=24768071",
        "https://web.archive.org/web/20201014022702/https://news.ycombinator.com/item?id=24762449",
        "https://web.archive.org/web/20201014022643/https://news.ycombinator.com/item?id=24766508",
        "https://web.archive.org/web/20201014022755/https://news.ycombinator.com/item?id=24762758",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24758772",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24761116",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24757333",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24755614",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24754662",
        "https://web.archive.org/web/20201013015309/https://news.ycombinator.com/item?id=24758020",
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24753283",
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24756902",
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24753564",
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24747667",
      ];
      return urls[Math.floor(Math.random() * urls.length)];
    },
    isUrlEqual: (url1: string, url2: string): boolean => {
      const getPath = (url: string) => {
        const archiveRegex = /https:\/\/web\.archive\.org\/web\/.*?\/(.*)/;
        const match = archiveRegex.exec(url);
        return match.length === 2 ? match[1] : null;
      };
      const path1 = getPath(url1);
      const path2 = getPath(url2);

      return path1 && path2 && path1 === path2;
    },
    parse: (scope: InjectScope): Text[] => {
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove()
      }

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
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2009/10/07/the-gervais-principle-or-the-office-according-to-the-office/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2010/07/26/a-big-little-idea-called-legibility/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2015/05/07/weaponized-sacredness/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2017/01/05/tendrils-of-mess-in-our-brains/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2017/08/17/the-premium-mediocre-life-of-maya-millennial/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2020/01/16/the-internet-of-beefs/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2018/11/28/the-digital-maginot-line/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2017/10/10/the-blockchain-man/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2015/09/24/samuel-becketts-guide-to-particles-and-antiparticles/",
        "https://web.archive.org/web/20200929120305/https://www.ribbonfarm.com/2016/05/12/artem-vs-predator/",
      ];

      return urls[Math.floor(Math.random() * urls.length)];
    },
    isUrlEqual: (url1: string, url2: string): boolean => {
      const getPath = (url: string) => {
        const archiveRegex = /https:\/\/web\.archive\.org\/web\/.*?\/(.*)/;
        const match = archiveRegex.exec(url);
        return match.length === 2 ? match[1] : null;
      };
      const path1 = getPath(url1);
      const path2 = getPath(url2);

      return path1 && path2 && path1 === path2;
    },
    parse: (scope: InjectScope): Text[] => {
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove()
      }

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
