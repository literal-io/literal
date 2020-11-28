import { DOMAIN, InjectScope, ParserInterface } from "./types";

export const parsers: { [domain: string]: ParserInterface } = {
  [DOMAIN.WIKIPEDIA]: {
    getUrls: () => [
      "https://web.archive.org/web/20200913110219/https://en.m.wikipedia.org/wiki/Cat_gap",
      "https://web.archive.org/web/20200816082257/https://en.m.wikipedia.org/wiki/Fossil_record",
      "https://web.archive.org/web/20201108164151/http://en.m.wikipedia.org/wiki/Napoleon",
      "https://web.archive.org/web/20201110120538/https://en.m.wikipedia.org/wiki/First_French_Empire",
      "https://web.archive.org/web/20201018092015/https://en.m.wikipedia.org/wiki/French_First_Republic",
      "https://web.archive.org/web/20201018092015/https://en.m.wikipedia.org/wiki/Reign_of_Terror",
      "https://web.archive.org/web/20200916071206/https://en.m.wikipedia.org/wiki/Fall_of_Maximilien_Robespierre",
      "https://web.archive.org/web/20201002005328/https://en.m.wikipedia.org/wiki/Purge",
      "https://web.archive.org/web/20201001123859/https://en.m.wikipedia.org/wiki/Night_of_the_Long_Knives",
      "https://web.archive.org/web/20201002013423/https://en.m.wikipedia.org/wiki/Paul_von_Hindenburg",
      "https://web.archive.org/web/20161111063316/https://en.m.wikipedia.org/wiki/Human_skeleton",
      "https://web.archive.org/web/20200922140454/https://en.m.wikipedia.org/wiki/Franco-Prussian_War",
      "https://web.archive.org/web/20201002081606/https://en.m.wikipedia.org/wiki/Kingdom_of_Prussia",
      "https://web.archive.org/web/20200805005128/https://en.m.wikipedia.org/wiki/Frederick_I,_Elector_of_Brandenburg",
      "https://web.archive.org/web/20161025050725/https://en.m.wikipedia.org/wiki/Human_iron_metabolism",
      "https://web.archive.org/web/20161020233813/https://en.m.wikipedia.org/wiki/Mammals",
      "https://web.archive.org/web/20161012192447/https://en.m.wikipedia.org/wiki/Organism",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Life",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Biological_process",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Bacteria",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Plant",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Science",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Synthetic_biology",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Stimulus_(physiology)",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Gaia_hypothesis",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Abiogenesis",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Fungi",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Earth",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Biological_organization",
      "https://web.archive.org/web/20161012143432/https://en.m.wikipedia.org/wiki/Physics",
    ],
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
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });

      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
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

      document.querySelectorAll(".toccolours").forEach((elem) => elem.remove());

      const textNodes = scope.getTextNodes(
        document.querySelector("#bodyContent")
      );

      return textNodes;
    },
    getBoundaryAncestorSelector: () => "section",
  },
  [DOMAIN.RIBBONFARM]: {
    getUrls: () => [
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
    ],
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
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });

      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });

      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      document
        .querySelectorAll(".after-post, .post-meta, .sharedaddy, fieldset")
        .forEach((el) => {
          el.remove();
        });

      return scope.getTextNodes(document.querySelector(".entry-content"));
    },
    getBoundaryAncestorSelector: () => "p",
  },
  [DOMAIN.PROJECT_GUTENBERG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201110155804/http://www.gutenberg.org/files/2701/2701-h/2701-h.htm",
      "https://web.archive.org/web/20201109153710/http://www.gutenberg.org/files/1946/1946-h/1946-h.htm",
      "https://web.archive.org/web/20200731222303/http://www.gutenberg.org/files/42324/42324-h/42324-h.htm",
      "https://web.archive.org/web/20201022141836/http://www.gutenberg.org/files/42671/42671-h/42671-h.htm",
      "https://web.archive.org/web/20201106022415/https://www.gutenberg.org/files/25344/25344-h/25344-h.htm",
      "https://web.archive.org/web/20201110000023/https://gutenberg.org/files/11/11-h/11-h.htm",
      "https://web.archive.org/web/20201106232638/http://www.gutenberg.org/files/5200/5200-h/5200-h.htm",
      "https://web.archive.org/web/20200523202814/https://www.gutenberg.org/files/1080/1080-h/1080-h.htm",
      "https://web.archive.org/web/20201107062000/http://www.gutenberg.org/files/844/844-h/844-h.htm",
    ],
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
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const textNodes = Array.from(document.querySelectorAll("p"))
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
    getBoundaryAncestorSelector: () => "p",
  },
  [DOMAIN.ACOUP]: {
    getUrls: () => [
      "https://web.archive.org/web/20201114132121/https://acoup.blog/2020/11/13/collections-why-military-history/",
      "https://web.archive.org/web/20201102224056/https://acoup.blog/2020/10/16/collections-iron-how-did-they-make-it-part-ivb-work-hardening-or-hardly-working/",
      "https://web.archive.org/web/20201112230613/https://acoup.blog/2020/10/09/collections-iron-how-did-they-make-it-part-iva-steel-yourself/",
      "https://web.archive.org/web/20201113190722/https://acoup.blog/2020/09/25/collections-iron-how-did-they-make-it-part-ii-trees-for-blooms/",
      "https://web.archive.org/web/20201104065529/https://acoup.blog/2020/09/18/collections-iron-how-did-they-make-it-part-i-mining/",
      "https://web.archive.org/web/20201113164438/https://acoup.blog/2020/09/11/miscellanea-my-thoughts-on-crusader-kings-iii/",
      "https://web.archive.org/web/20201031115300/https://acoup.blog/2020/09/04/collections-bread-how-did-they-make-it-addendum-rice/",
      "https://web.archive.org/web/20201029202703/https://acoup.blog/2020/08/21/collections-bread-how-did-they-make-it-part-iv-markets-and-non-farmers/",
      "https://web.archive.org/web/20201025220802/https://acoup.blog/2020/08/06/collections-bread-how-did-they-make-it-part-iii-actually-farming/",
      "https://web.archive.org/web/20201018005620/https://acoup.blog/2020/07/31/collections-bread-how-did-they-make-it-part-ii-big-farms/",
      "https://web.archive.org/web/20201031232803/https://acoup.blog/2020/07/24/collections-bread-how-did-they-make-it-part-i-farmers/",
      "https://web.archive.org/web/20201003051805/https://acoup.blog/2020/07/09/collections-how-your-history-gets-made/",
      "https://web.archive.org/web/20201114232500/https://acoup.blog/2020/07/03/collections-the-practical-case-on-why-we-need-the-humanities/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const actionBar = document.getElementById("actionbar");
      if (actionBar) {
        actionBar.remove();
      }

      return scope.getTextNodes(document.querySelector(".entry-content"));
    },
  },
  [DOMAIN.ACM_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201016173104/https://cacm.acm.org/blogs/blog-cacm/248022-what-everyone-knows-and-what-no-one-knows/fulltext",
      "https://web.archive.org/web/20201101183146/https://cacm.acm.org/blogs/blog-cacm/247225-things-to-do-to-an-algorithm/fulltext",
      "https://web.archive.org/web/20200730190515/https://cacm.acm.org/blogs/blog-cacm/245277-the-remote-revolution-has-to-be-driven-by-output-not-salaries/fulltext",
      "https://web.archive.org/web/20200924021440/https://cacm.acm.org/blogs/blog-cacm/245226-is-a-nearly-zero-cost-model-plausible-for-science-and-engineering-programs/fulltext",
      "https://web.archive.org/web/20200730190844/https://cacm.acm.org/blogs/blog-cacm/244736-hacking-the-axis/fulltext",
      "https://web.archive.org/web/20200730191100/https://cacm.acm.org/blogs/blog-cacm/244499-the-covid-catalyst/fulltext",
      "https://web.archive.org/web/20200730194435/https://cacm.acm.org/blogs/blog-cacm/244379-holding-a-conference-online-and-live-due-to-covid-19/fulltext",
      "https://web.archive.org/web/20200730191100/https://cacm.acm.org/blogs/blog-cacm/244499-the-covid-catalyst/fulltext",
      "https://web.archive.org/web/20200730194435/https://cacm.acm.org/blogs/blog-cacm/244379-holding-a-conference-online-and-live-due-to-covid-19/fulltext",
      "https://web.archive.org/web/20200815172323/https://cacm.acm.org/blogs/blog-cacm/244188-computational-thinking-or-computational-teamwork/fulltext",
      "https://web.archive.org/web/20200730194751/https://cacm.acm.org/blogs/blog-cacm/244198-would-there-be-computers-without-easter/fulltext",
      "https://web.archive.org/web/20200725144609/https://cacm.acm.org/blogs/blog-cacm/243882-the-asplos-2020-online-conference-experience/fulltext",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const textNodes = Array.from(
        document.querySelectorAll("#articleFullText > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.PAUL_GRAHAM_BLOG]: {
    getUrls: () => [
      "http://web.archive.org/web/20201108072840/http://paulgraham.com/early.html",
      "http://web.archive.org/web/20201108150533/http://paulgraham.com/wtax.html",
      "http://web.archive.org/web/20201112063521/http://paulgraham.com/conformism.html",
      "http://web.archive.org/web/20201107110007/http://paulgraham.com/orth.html",
      "http://web.archive.org/web/20201030151043/http://paulgraham.com/cred.html",
      "http://web.archive.org/web/20201107170124/http://paulgraham.com/useful.html",
      "http://web.archive.org/web/20201030170158/http://paulgraham.com/noob.html",
      "http://web.archive.org/web/20201108023057/http://www.paulgraham.com/fh.html",
      "http://web.archive.org/web/20201104154753/http://paulgraham.com/mod.html",
      "http://web.archive.org/web/20201030174931/http://paulgraham.com/fp.html",
      "http://web.archive.org/web/20201111185958/http://www.paulgraham.com/kids.html",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector("font"));
    },
  },
  [DOMAIN.SEC]: {
    getUrls: () => [
      "https://web.archive.org/web/20201030000827/https://www.sec.gov/Archives/edgar/data/1810806/000119312520227862/d908875ds1.htm",
      "https://web.archive.org/web/20201116220437/https://www.sec.gov/Archives/edgar/data/1559720/000119312520294801/d81668ds1.htm",
      "https://web.archive.org/web/20201030000827/https://www.sec.gov/Archives/edgar/data/1810806/000119312520227862/d908875ds1.htm",
      "https://web.archive.org/web/20201109224957/https://www.sec.gov/Archives/edgar/data/1477720/000119312520228462/d855753ds1.htm",
      "https://web.archive.org/web/20201108070128/https://www.sec.gov/Archives/edgar/data/1643269/000119312520227201/d821436ds1.htm",
      "https://web.archive.org/web/20201115111400/https://www.sec.gov/Archives/edgar/data/1800667/000119312520228195/d841831ds1.htm",
      "https://web.archive.org/web/20201116041505/https://www.sec.gov/Archives/edgar/data/1585521/000119312519083351/d642624ds1.htm",
      "https://web.archive.org/web/20201103002558/https://www.sec.gov/Archives/edgar/data/1477333/000119312519222176/d735023ds1.htm",
      "https://web.archive.org/web/20201108031228/https://www.sec.gov/Archives/edgar/data/1561550/000119312519227783/d745413ds1.htm",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      document.querySelectorAll("table").forEach((elem) => elem.remove());

      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return (
        Array.from(document.querySelectorAll("center"))
          .map(scope.getTextNodes)
          //@ts-ignore: this should work fine
          .flat()
      );
    },
  },
  [DOMAIN.THE_DIFF_SUBSTACK]: {
    getUrls: () => [
      "https://web.archive.org/web/20201101035820/https://diff.substack.com/p/engineering-a-conglomerate",
      "https://web.archive.org/web/20201110155008/https://diff.substack.com/p/surfing-the-right-s-curve",
      "https://web.archive.org/web/20201101023202/https://diff.substack.com/p/big-tech-at-the-end-of-history",
      "https://web.archive.org/web/20200920022132/https://diff.substack.com/p/banking-when-you-cant-bank-on-anything-8a5",
      "https://web.archive.org/web/20201115192208/https://diff.substack.com/p/banking-when-you-cant-bank-on-anything",
      "https://web.archive.org/web/20201101031536/https://diff.substack.com/p/ant-group-and-chinas-fitful-convergence",
      "https://web.archive.org/web/20201113054259/https://diff.substack.com/p/business-model-meta-models",
      "https://web.archive.org/web/20201114001615/https://diff.substack.com/p/how-bubbles-and-megaprojects-parallelize",
      "https://web.archive.org/web/20201110060533/https://diff.substack.com/p/big-tech-sees-like-a-state",
      "https://web.archive.org/web/20201101023516/https://diff.substack.com/p/hardware-as-a-service",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector("article"));
    },
  },
  [DOMAIN.PETER_TURCHIN_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201021042338/http://peterturchin.com/cliodynamica/the-double-helix-of-inequality-and-well-being/",
      "https://web.archive.org/web/20201002061249/http://peterturchin.com/cliodynamica/strange-disappearance/",
      "https://web.archive.org/web/20200923132247/http://peterturchin.com/cliodynamica/the-strange-disappearance-of-cooperation-in-america-ii/",
      "https://web.archive.org/web/20201021053127/http://peterturchin.com/cliodynamica/the-road-to-disunion/",
      "https://web.archive.org/web/20200923163044/http://peterturchin.com/cliodynamica/below-the-surface-the-structural-demographic-roots-of-the-current-political-crisis/",
      "https://web.archive.org/web/20201005014436/http://peterturchin.com/cliodynamica/bimodal-lawyers-how-extreme-competition-breeds-extreme-inequality/",
      "https://web.archive.org/web/20200923161501/http://peterturchin.com/cliodynamica/living-without-a-state/",
      "https://web.archive.org/web/20200925105439/http://peterturchin.com/cliodynamica/why-is-haiti-so-poor/",
      "https://web.archive.org/web/20201021052540/http://peterturchin.com/cliodynamica/deep-roots/",
      "https://web.archive.org/web/20201021040452/http://peterturchin.com/cliodynamica/an-imperfect-time-machine/",
      "https://web.archive.org/web/20200808204723/http://peterturchin.com/cliodynamica/getting-to-norway/",
      "https://web.archive.org/web/20200921125112/http://peterturchin.com/cliodynamica/paradoxes-of-the-nordic-model-i/",
      "https://web.archive.org/web/20200923122713/http://peterturchin.com/cliodynamica/paradoxes-of-the-nordic-model-ii/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const textNodes = Array.from(
        document.querySelectorAll(".single-content > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.GITHUB_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201101121117/https://github.blog/2020-10-29-getting-started-with-devops-automation/",
      "https://web.archive.org/web/20201106191702/https://github.blog/2020-10-29-making-github-ci-workflow-3x-faster/",
      "https://web.archive.org/web/20201109172045/https://github.blog/2020-10-29-building-github-introduction/",
      "https://web.archive.org/web/20201101033644/https://github.blog/2020-10-15-how-to-get-your-organization-started-with-containerized-deployments/",
      "https://web.archive.org/web/20201101053934/https://github.blog/2020-10-09-devops-cloud-testing/",
      "https://web.archive.org/web/20201020002615/https://github.blog/2020-09-02-github-availability-report-august-2020/",
      "https://web.archive.org/web/20201104182255/https://github.blog/2020-08-25-upgrading-github-to-ruby-2-7/",
      "https://web.archive.org/web/20200928111940/https://github.blog/2020-08-18-introducing-the-rally-github-integration/",
      "https://web.archive.org/web/20201101230810/https://github.blog/2020-08-13-why-write-adrs/",
      "https://web.archive.org/web/20200807192251/https://github.blog/2020-07-02-how-we-launched-docs-github-com/",
      "https://web.archive.org/web/20201026082657/https://github.blog/2020-06-18-introducing-github-super-linter-one-linter-to-rule-them-all/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector(".post__content"));
    },
  },
  [DOMAIN.GWERN_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201111164827/http://www.gwern.net/Silk-Road",
      "https://web.archive.org/web/20201108092305/http://www.gwern.net/Modafinil",
      "https://web.archive.org/web/20201109035329/http://www.gwern.net/LSD-microdosing",
      "https://web.archive.org/web/20201108182155/https://www.gwern.net/Zeo",
      "https://web.archive.org/web/20201108162226/http://www.gwern.net/DNB-FAQ",
      "https://web.archive.org/web/20201108100313/http://www.gwern.net/Spaced-repetition",
      "https://web.archive.org/web/20201108001024/https://www.gwern.net/Death-Note-Anonymity",
      "https://web.archive.org/web/20201106215038/https://www.gwern.net/Complement",
      "https://web.archive.org/web/20201112012619/http://www.gwern.net/Google-shutdowns",
      "https://web.archive.org/web/20201108002844/https://www.gwern.net/Ads",
      "https://web.archive.org/web/20201111212800/https://www.gwern.net/Tanks",
      "https://web.archive.org/web/20201109025321/http://www.gwern.net/Self-decrypting-files",
      "https://web.archive.org/web/20201108105242/http://www.gwern.net/Archiving-URLs",
      "https://web.archive.org/web/20201108091322/https://www.gwern.net/Terrorism-is-not-about-Terror",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      document.querySelectorAll("table,img,code").forEach((elem) => {
        elem.remove();
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector("#markdownBody"));
    },
  },
  [DOMAIN.NATURE]: {
    getUrls: () => [
      "https://web.archive.org/web/20200606090532/https://www.nature.com/articles/d41586-020-01567-3",
      "https://web.archive.org/web/20200607011244/https://www.nature.com/articles/d41586-020-01570-8",
      "https://web.archive.org/web/20200607132331/https://www.nature.com/articles/d41586-020-01566-4",
      "https://web.archive.org/web/20200606123933/https://www.nature.com/articles/d41586-020-01485-4",
      "https://web.archive.org/web/20200606035115/https://www.nature.com/articles/d41586-020-01483-6",
      "https://web.archive.org/web/20200606062626/https://www.nature.com/articles/d41586-020-01455-w",
      "https://web.archive.org/web/20200607132232/https://www.nature.com/articles/d41586-020-01484-5",
      "https://web.archive.org/web/20201116094312/https://www.nature.com/articles/d41586-020-02791-7",
      "https://web.archive.org/web/20201104164631/https://www.nature.com/articles/d41586-020-02831-2",
      "https://web.archive.org/web/20201105053618/https://www.nature.com/articles/d41586-020-02750-2",
      "https://web.archive.org/web/20201102222655/https://www.nature.com/articles/d41586-020-02712-8",
      "https://web.archive.org/web/20201102222457/https://www.nature.com/articles/d41586-020-02526-8",
      "https://web.archive.org/web/20201105060729/https://www.nature.com/articles/d41586-020-02941-x",
      "https://web.archive.org/web/20201104153623/https://www.nature.com/articles/d41586-020-02942-w",
    ],
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
    getScrollOffsetHeight: () => {
      const header = document.querySelector(".default-header");
      if (header) {
        return header.clientHeight;
      }
      return 0;
    },
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      // remove cookie consent header, subscription prompt
      document
        .querySelectorAll(".optanon-alert-box-wrapper,.c-site-messages")
        .forEach((el) => {
          el.remove();
        });

      const textNodes = Array.from(
        document.querySelectorAll(".article__body > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.TONSKY_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201030150935/https://tonsky.me/blog/tech-sucks/",
      "https://web.archive.org/web/20201030151018/https://tonsky.me/blog/sublime/",
      "https://web.archive.org/web/20201031035444/https://tonsky.me/blog/alpha/",
      "https://web.archive.org/web/20201115090202/https://tonsky.me/blog/monitors/",
      "https://web.archive.org/web/20201112214417/https://tonsky.me/blog/syncthing/",
      "https://web.archive.org/web/20201030084230/https://tonsky.me/blog/form-cleanup/",
      "https://web.archive.org/web/20201101073426/https://tonsky.me/blog/utils/",
      "https://web.archive.org/web/20201030154628/https://tonsky.me/blog/performance-first/",
      "https://web.archive.org/web/20201112184906/https://tonsky.me/blog/swiftui/",
      "https://web.archive.org/web/20201110141858/https://tonsky.me/blog/good-times-weak-men/",
      "https://web.archive.org/web/20201027223923/https://tonsky.me/blog/pedestal/",
      "https://web.archive.org/web/20201023085431/https://tonsky.me/blog/uberdeps/",
      "https://web.archive.org/web/20201107224629/https://tonsky.me/blog/hiring/",
      "https://web.archive.org/web/20201108001701/https://tonsky.me/blog/github-redesign/",
      "https://web.archive.org/web/20201117152504/https://tonsky.me/blog/skija/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector(".post"));
    },
  },
  [DOMAIN.FIRST_ROUND_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201101032003if_/https://firstround.com/review/your-marketing-org-is-slow-heres-a-framework-to-move-faster/",
      "https://web.archive.org/web/20201101031841if_/https://firstround.com/review/the-40-best-questions-to-ask-in-an-interview-how-to-go-deeper-than-whats-the-culture-like/",
      "https://web.archive.org/web/20201101030735if_/https://firstround.com/review/drive-growth-by-picking-the-right-lane-a-customer-acquisition-playbook-for-consumer-startups/",
      "https://web.archive.org/web/20201101004404if_/https://firstround.com/review/use-this-startups-playbook-for-running-impactful-virtual-offsites/",
      "https://web.archive.org/web/20201101035754if_/https://firstround.com/review/6-small-steps-for-handling-the-emotional-ups-and-downs-at-work/",
      "https://web.archive.org/web/20201101033742if_/https://firstround.com/review/this-vp-is-doing-things-differently-in-the-product-org-heres-his-playbook/",
      "https://web.archive.org/web/20201103163043if_/https://firstround.com/review/the-ultimate-guide-to-the-founding-designer-role/",
      "https://web.archive.org/web/20201101002428if_/https://firstround.com/review/a-founders-guide-to-writing-well/",
      "https://web.archive.org/web/20201101144832if_/https://firstround.com/review/how-to-take-personal-development-off-the-backburner-tactical-frameworks-for-leveling-up/",
      "https://web.archive.org/web/20201108050526if_/https://firstround.com/review/the-managers-guide-to-inclusive-leadership-small-habits-that-make-a-big-impact/",
      "https://web.archive.org/web/20201106151417if_/https://firstround.com/review/hit-the-emotional-gym-the-founders-framework-for-emotional-fitness/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector("._Content__"));
    },
  },
  [DOMAIN.CRYTOGRAPHY_ENGINEERING_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201118013045/https://blog.cryptographyengineering.com/2020/11/16/ok-google-please-publish-your-dkim-secret-keys/",
      "https://web.archive.org/web/20201116205201/https://blog.cryptographyengineering.com/2016/11/24/android-n-encryption/",
      "https://web.archive.org/web/20201116205154/https://blog.cryptographyengineering.com/2020/07/10/a-few-thoughts-about-signals-secure-value-recovery/",
      "https://web.archive.org/web/20201116205149/https://blog.cryptographyengineering.com/2018/09/23/why-im-leaving-chrome/",
      "https://web.archive.org/web/20201116205157/https://blog.cryptographyengineering.com/2014/08/13/whats-matter-with-pgp/",
      "https://web.archive.org/web/20201116205212/https://blog.cryptographyengineering.com/2013/03/12/attack-of-week-rc4-is-kind-of-broken-in/",
      "https://web.archive.org/web/20201116205208/https://blog.cryptographyengineering.com/2014/11/27/zero-knowledge-proofs-illustrated-primer/",
      "https://web.archive.org/web/20201116205157/https://blog.cryptographyengineering.com/2018/10/19/lets-talk-about-pake/",
      "https://web.archive.org/web/20201109215210/https://blog.cryptographyengineering.com/2012/05/19/how-to-choose-authenticated-encryption/",
      "https://web.archive.org/web/20201116205159/https://blog.cryptographyengineering.com/2020/08/12/attack-of-the-week-voice-calls-in-lte/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const textNodes = Array.from(
        document.querySelectorAll(".entry-content > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.PARIS_REVIEW]: {
    getUrls: () => [
      "https://web.archive.org/web/20201117094532/https://www.theparisreview.org/blog/2020/11/16/we-are-built-to-forget/",
      "https://web.archive.org/web/20201111230844/https://www.theparisreview.org/blog/2020/11/11/inside-the-american-snow-dome/",
      "https://web.archive.org/web/20190426144812/https://www.theparisreview.org/blog/2019/04/22/the-unknowable-artist-stephane-mandelbaum/",
      "https://web.archive.org/web/20201101061054/https://www.theparisreview.org/blog/2019/11/26/on-desolation-vija-celminss-gray/",
      "https://web.archive.org/web/20200901210012/https://www.theparisreview.org/blog/2020/09/01/wait-what-year-is-this/",
      "https://web.archive.org/web/20201111222716/https://www.theparisreview.org/blog/2020/11/10/redux-the-feeling-of-an-airplane-crashing/",
      "https://web.archive.org/web/20201101065111/https://www.theparisreview.org/blog/2020/09/28/feminize-your-canon-alice-dunbar-nelson/",
      "https://web.archive.org/web/20201111080359/https://www.theparisreview.org/blog/2020/11/10/re-covered-living-through-history/",
      "https://web.archive.org/web/20201112085901/https://www.theparisreview.org/blog/2020/11/09/the-art-of-distance-no-33/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      document.querySelectorAll('[role="dialog"]').forEach((elem) => {
        elem.remove();
      });

      return scope.getTextNodes(document.querySelector(".article-body"));
    },
  },
  [DOMAIN.COCKROACH_DB_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201118000512/https://www.cockroachlabs.com/blog/why-i-left-ibm/",
      "https://web.archive.org/web/20200921030026/https://www.cockroachlabs.com/blog/alter-column-type/",
      "https://web.archive.org/web/20200921015819/https://www.cockroachlabs.com/blog/internal-mobility-program/",
      "https://web.archive.org/web/20200831131909/https://www.cockroachlabs.com/blog/full-text-indexing-search/",
      "https://web.archive.org/web/20200907124722/https://www.cockroachlabs.com/blog/database-consistency/",
      "https://web.archive.org/web/20201004002234/https://www.cockroachlabs.com/blog/kubernetes-saas-implementation/",
      "https://web.archive.org/web/20200921023137/https://www.cockroachlabs.com/blog/pebble-rocksdb-kv-store/",
      "https://web.archive.org/web/20200921141220/https://www.cockroachlabs.com/blog/building-support-for-java-orm-hibernate-in-cockroachdb/",
      "https://web.archive.org/web/20201001204653/https://www.cockroachlabs.com/blog/true-cost-cloud-database/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      document.querySelectorAll('[role="dialog"]').forEach((elem) => {
        elem.remove();
      });

      return scope.getTextNodes(document.querySelector(".single-blog-content"));
    },
  },
  [DOMAIN.LAPHAMS_QUARTERLY]: {
    getUrls: () => [
      "https://web.archive.org/web/20201118153710/https://www.laphamsquarterly.org/roundtable/hotel-heart-hudson-river-school",
      "https://web.archive.org/web/20201116153458/https://www.laphamsquarterly.org/roundtable/more-perfect-union",
      "https://web.archive.org/web/20201112014247/https://www.laphamsquarterly.org/animals/our-orgiastic-future",
      "https://web.archive.org/web/20201109035606/https://www.laphamsquarterly.org/fear/fear-arrival",
      "https://web.archive.org/web/20170801070934/http://www.laphamsquarterly.org/revolutions/no-smoke-camilo",
      "https://web.archive.org/web/20201109015335/https://www.laphamsquarterly.org/rule-law/immune-law",
      "https://web.archive.org/web/20201118025949/https://www.laphamsquarterly.org/discovery/queer-theorem",
      "https://web.archive.org/web/20201108162644/https://www.laphamsquarterly.org/fashion/crusader-chic",
      "https://web.archive.org/web/20201108135121/https://www.laphamsquarterly.org/night/fading-stars-constellation",
      "https://web.archive.org/web/20201112032205/https://www.laphamsquarterly.org/death/last-meals",
      "https://web.archive.org/web/20200328172939/https://www.laphamsquarterly.org/politics/great-rift",
      "https://web.archive.org/web/20201109035717/https://www.laphamsquarterly.org/states-mind/person-ape",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector(".content-wrapper"));
    },
  },
  [DOMAIN.GOOGLE_AI_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20200810115608/https://ai.googleblog.com/2020/01/reformer-efficient-transformer.html",
      "https://web.archive.org/web/20200810113904/https://ai.googleblog.com/2020/01/encode-tag-and-realize-controllable-and.html",
      "https://web.archive.org/web/20200725070512/https://ai.googleblog.com/2020/01/announcing-third-workshop-and-challenge.html",
      "https://web.archive.org/web/20200810122343/https://ai.googleblog.com/2020/01/towards-conversational-agent-that-can.html",
      "https://web.archive.org/web/20200810122321/https://ai.googleblog.com/2020/01/releasing-drosophila-hemibrain.html",
      "https://web.archive.org/web/20201112023705/https://ai.googleblog.com/2020/11/improving-on-device-speech-recognition.html",
      "https://web.archive.org/web/20201111062634/https://ai.googleblog.com/2020/10/background-features-in-google-meet.html",
      "https://web.archive.org/web/20201111121416/https://ai.googleblog.com/2020/10/experimenting-with-automatic-video.html",
      "https://web.archive.org/web/20201105013229/https://ai.googleblog.com/2020/10/estimating-impact-of-training-data-with.html",
      "https://web.archive.org/web/20201101095518/https://ai.googleblog.com/2020/10/rethinking-attention-with-performers.html",
      "https://web.archive.org/web/20201101133433/https://ai.googleblog.com/2020/10/announcing-2020-award-for-inclusion.html",
      "https://web.archive.org/web/20201108021518/https://ai.googleblog.com/2020/10/recreating-historical-streetscapes.html",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }
      const banner = document.querySelector("#cookieChoiceInfo");
      if (banner) {
        banner.remove();
      }

      return scope.getTextNodes(document.querySelector(".post-content"));
    },
  },
  [DOMAIN.NPR_TEXT]: {
    getUrls: () => [
      "https://web.archive.org/web/20201119010430/https://text.npr.org/936268845",
      "https://web.archive.org/web/20201118182259/https://text.npr.org/936196364",
      "https://web.archive.org/web/20201118162308/https://text.npr.org/935730100",
      "https://web.archive.org/web/20201118212300/https://text.npr.org/936282353",
      "https://web.archive.org/web/20201118222309/https://text.npr.org/936177167",
      "https://web.archive.org/web/20201101014312/https://text.npr.org/816707182",
      "https://web.archive.org/web/20201107042323/https://text.npr.org/932364567",
      "https://web.archive.org/web/20201107042312/https://text.npr.org/932369007",
      "https://web.archive.org/web/20201107062329/https://text.npr.org/932383604",
      "https://web.archive.org/web/20201101022118/https://text.npr.org/928392673",
      "https://web.archive.org/web/20201107062336/https://text.npr.org/931891674",
      "https://web.archive.org/web/20201107072307/https://text.npr.org/932178353",
      "https://web.archive.org/web/20201105162324/https://text.npr.org/931378189",
      "https://web.archive.org/web/20201106172323/https://text.npr.org/931888744",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }
      const banner = document.querySelector("#cookieChoiceInfo");
      if (banner) {
        banner.remove();
      }

      return scope.getTextNodes(
        document.querySelector(".paragraphs-container")
      );
    },
  },
  [DOMAIN.BOOK_FORUM]: {
    getUrls: () => [
      "https://web.archive.org/web/20201116162234/https://www.bookforum.com/print/2704/the-enduring-songcraft-of-dolly-parton-24261",
      "https://web.archive.org/web/20201111152932mp_/https://www.bookforum.com/print/2703/a-history-of-the-human-choices-that-led-to-one-of-the-century-s-worst-natural-disasters-24175",
      "https://web.archive.org/web/20201113044409mp_/https://www.bookforum.com/print/2001/a-new-profanely-titled-tract-highlights-the-shortcomings-of-the-cottage-industry-of-pop-philosophizing-11216",
      "https://web.archive.org/web/20201115200826mp_/https://www.bookforum.com/print/2703/necessary-errors-24174",
      "https://web.archive.org/web/20201109060602mp_/https://www.bookforum.com/print/2703/two-french-thinkers-offer-a-manual-for-a-radically-altered-future-24172",
      "https://web.archive.org/web/20201109163915mp_/https://www.bookforum.com/print/2704/the-chaotic-quest-to-mythologize-america-s-past-24250",
      "https://web.archive.org/web/20201113163305mp_/https://www.bookforum.com/papertrail/mathew-ingram-looks-at-the-possibility-of-a-trump-media-venture-jonathan-franzen-is-writing-a-trilogy-24279",
      "https://web.archive.org/web/20201105020844mp_/https://www.bookforum.com/print/2704/confronting-the-age-of-hate-in-america-24245",
      "https://web.archive.org/web/20201102155036mp_/https://www.bookforum.com/print/2704/jeffrey-toobin-s-account-of-the-trump-impeachment-24242",
      "https://web.archive.org/web/20201116040204mp_/https://www.bookforum.com/print/2704/the-martin-papers-24240",
      "https://web.archive.org/web/20201101053837mp_/https://www.bookforum.com/culture/an-excerpt-from-black-spartacus-the-epic-life-of-toussaint-louverture-24235",
    ],
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
    getScrollOffsetHeight: () => {
      const header = document.querySelector(".header");
      if (header) {
        return header.clientHeight;
      }
      return 0;
    },
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const banner = document.querySelector(".paywall-counter");
      if (banner) {
        banner.remove();
      }

      const overlay = document.querySelector(".overlay");
      if (overlay) {
        overlay.remove();
      }

      const textNodes = Array.from(
        document.querySelectorAll(".blog-article__content > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.NVIDIA_NEWS]: {
    getUrls: () => [
      "https://web.archive.org/web/20201118205712/https://nvidianews.nvidia.com/news/nvidia-doubles-down-announces-a100-80gb-gpu-supercharging-worlds-most-powerful-gpu-for-ai-supercomputing",
      "https://web.archive.org/web/20201116180741/https://nvidianews.nvidia.com/news/nvidia-announces-mellanox-infiniband-for-exascale-ai-supercomputing",
      "https://web.archive.org/web/20201116142725/https://nvidianews.nvidia.com/news/nvidia-dgx-station-a100-offers-researchers-ai-data-center-in-a-box",
      "https://web.archive.org/web/20201113190524/https://nvidianews.nvidia.com/news/hyundai-motor-group-selects-nvidia-drive-infotainment-and-ai-platform-for-all-future-hyundai-kia-and-genesis-models",
      "https://web.archive.org/web/20201113173438/https://nvidianews.nvidia.com/news/nvidia-names-aarti-shah-to-board-of-directors",
      "https://web.archive.org/web/20201101011348/https://nvidianews.nvidia.com/news/nvidia-sets-conference-call-for-third-quarter-financial-results-6819118",
      "https://web.archive.org/web/20201031234651/https://nvidianews.nvidia.com/news/nvidia-smashes-performance-records-on-ai-inference",
      "https://web.archive.org/web/20201101023803/https://nvidianews.nvidia.com/news/cineca-to-build-worlds-fastest-ai-supercomputer-with-nvidia-and-atos",
      "https://web.archive.org/web/20201104205602/https://nvidianews.nvidia.com/news/nvidia-introduces-new-family-of-bluefield-dpus-to-bring-breakthrough-networking-storage-and-security-performance-to-every-data-center",
      "https://web.archive.org/web/20201105173506/https://nvidianews.nvidia.com/news/nvidia-building-uks-most-powerful-supercomputer-dedicated-to-ai-research-in-healthcare",
      "https://web.archive.org/web/20201118092721/https://nvidianews.nvidia.com/news/nvidia-unveils-jetson-nano-2gb-the-ultimate-ai-and-robotics-starter-kit-for-students-educators-robotics-hobbyists",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    getScrollOffsetHeight: () => {
      const header = document.querySelector(".global-nav");
      if (header) {
        return header.clientHeight;
      }
      return 0;
    },
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const banner = document.querySelector("#cookiePolicy-layer");
      if (banner) {
        banner.remove();
      }

      return scope.getTextNodes(document.querySelector(".article-body"));
    },
  },
  [DOMAIN.GEMSBOK_BLOG]: {
    getUrls: () => [
      "https://web.archive.org/web/20201022071119/https://thegemsbok.com/art-reviews-and-articles/mid-week-mission-portal-valve/",
      "https://web.archive.org/web/20201030134911/https://thegemsbok.com/art-reviews-and-articles/book-reviews-tuesday-tome-the-stranger-albert-camus/",
      "https://web.archive.org/web/20201030111631/https://thegemsbok.com/art-reviews-and-articles/mid-week-mission-terraria-re-logic/",
      "https://web.archive.org/web/20201022063543/https://thegemsbok.com/art-reviews-and-articles/tuesday-tome-clockwork-orange-anthony-burgess/",
      "https://web.archive.org/web/20201030143504/https://thegemsbok.com/art-reviews-and-articles/philosophy-articles-friday-phil-colin-radford-paradox-of-fiction/",
      "https://web.archive.org/web/20201030081951/https://thegemsbok.com/art-reviews-and-articles/friday-phil-anthropic-principle-carl-sagan/",
      "https://web.archive.org/web/20201030074457/https://thegemsbok.com/art-reviews-and-articles/thursday-theater-arrival-denis-villeneuve/",
      "https://web.archive.org/web/20201030151855/https://thegemsbok.com/art-reviews-and-articles/philosophy-articles-friday-phil-free-will-foreknowledge/",
      "https://web.archive.org/web/20201029173625/https://thegemsbok.com/art-reviews-and-articles/philosophy-articles-friday-phil-free-will-determinism-compatibilism/",
      "https://web.archive.org/web/20201030115406/https://thegemsbok.com/art-reviews-and-articles/friday-phil-friedrich-nietzsche-truth-genealogy/",
      "https://web.archive.org/web/20201030103945/https://thegemsbok.com/art-reviews-and-articles/friday-phil-pascals-wager/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      document.querySelectorAll(".a2a_kit").forEach((el) => el.remove());
      document
        .querySelectorAll(".share_box_label_horizontal")
        .forEach((el) => el.remove());

      const textNodes = Array.from(
        document.querySelectorAll(".entry-content > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.THE_MARKUP]: {
    getUrls: () => [
      "https://web.archive.org/web/20201117130056/https://themarkup.org/election-2020/2020/11/17/targeting-trump-fans-qanon-ad-slips-through-facebooks-filters",
      "https://web.archive.org/web/20201117170038/https://themarkup.org/google-the-giant/2020/11/10/introducing-simple-search",
      "https://web.archive.org/web/20201115221310/https://themarkup.org/ask-the-markup/2020/11/12/how-private-is-my-pay-app",
      "https://web.archive.org/web/20201113030111/https://themarkup.org/election-2020/2020/11/05/tech-platforms-election-moderation-promises-twitter-facebook-youtube",
      "https://web.archive.org/web/20201119154521/https://themarkup.org/election-2020/2020/11/03/voting-machines-security-progress-by-states",
      "https://web.archive.org/web/20201118121357/https://themarkup.org/election-2020/2020/11/02/election-misinformation-tech-social-media-platform-moderation",
      "https://web.archive.org/web/20201119201707/https://themarkup.org/election-2020/2020/10/30/prop-22-california-gig-workers-uber-lyft-doordash-instacart",
      "https://web.archive.org/web/20201117172614/https://themarkup.org/election-2020/2020/10/29/facebook-political-ad-targeting-algorithm-prices-trump-biden",
      "https://web.archive.org/web/20201101023139/https://themarkup.org/google-the-giant/2020/10/20/google-antitrust-lawsuit-markup-investigations",
      "https://web.archive.org/web/20201025085448/https://themarkup.org/ask-the-markup/2020/10/20/0-electronics-right-to-repair-ventilators-iphone",
      "https://web.archive.org/web/20201025085453/https://themarkup.org/google-the-giant/2020/10/15/big-tech-antitrust-google-nondiscrimination-enforcement",
      "https://web.archive.org/web/20201020064219/https://themarkup.org/coronavirus/2020/10/13/remote-exam-software-failures-privacy",
      "https://web.archive.org/web/20201031223856/https://themarkup.org/locked-out/2020/10/06/zombie-criminal-records-housing-background-checks",
      "https://web.archive.org/web/20201119134920/https://themarkup.org/google-the-giant/2020/11/19/as-antitrust-pressure-mounts-google-to-pull-back-benefit-to-news-sites-that-adopted-its-preferred-mobile-technology",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const textNodes = Array.from(
        document.querySelectorAll(".article-body > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.STANFORD_NEWS]: {
    getUrls: () => [
      "https://web.archive.org/web/20201116032657/https://news.stanford.edu/2020/11/13/undersea-origins-earths-mysterious-love-waves/",
      "https://web.archive.org/web/20201113170214/https://news.stanford.edu/2020/11/12/religious-clashes-india-sparked-scholars-interest-peace/",
      "https://web.archive.org/web/20201117121920/https://news.stanford.edu/2020/11/11/lessons-handling-covid-19-another-animal-virus/",
      "https://web.archive.org/web/20201117031627/https://news.stanford.edu/2020/11/13/faculty-senate-hears-presentation-universitys-long-range-vision/",
      "https://web.archive.org/web/20201116010348/https://news.stanford.edu/2020/11/10/computer-model-can-predict-covid-19s-spread/",
      "https://web.archive.org/web/20201111102637/https://news.stanford.edu/2020/11/09/forecasting-ecosystem-changes-dna/",
      "https://web.archive.org/web/20201112020822/https://news.stanford.edu/2020/11/10/special-forces-veteran-stanford-scholar-applies-data-scholarship-conflict/",
      "https://web.archive.org/web/20201115214103/https://news.stanford.edu/2020/11/13/long-range-vision-annual-report-details-2019-2020-activities/",
      "https://web.archive.org/web/20201118154059/https://news.stanford.edu/2020/11/10/researchers-explain-eruption-jupiters-moon-europa/",
      "https://web.archive.org/web/20201109131710/https://news.stanford.edu/2020/11/09/innovative-tools-bolster-stanfords-covid-19-surveillance-testing-program/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    getScrollOffsetHeight: () => {
      const elem = document.querySelector(".navbar");
      if (elem) {
        return elem.parentElement.clientHeight;
      }
      return 0;
    },
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector("#story-content"));
    },
  },
  [DOMAIN.MIT_NEWS]: {
    getUrls: () => [
      "https://web.archive.org/web/20201118160536/https://news.mit.edu/2020/bhavik-nagda-1118",
      "https://web.archive.org/web/20201119010044/https://news.mit.edu/2020/reasons-nuclear-overruns-1118",
      "https://web.archive.org/web/20201117160547/https://news.mit.edu/2020/kerri-cahoy-mini-satellite-1117",
      "https://web.archive.org/web/20201118172600/https://news.mit.edu/2020/work-of-future-final-report-1117",
      "https://web.archive.org/web/20201118214548/https://news.mit.edu/2020/powering-through-coming-energy-transition-1118",
      "https://web.archive.org/web/20201118202225/https://news.mit.edu/2020/phiala-shanahan-receives-kenneth-wilson-award-1118",
      "https://web.archive.org/web/20201118195123/https://news.mit.edu/2020/identifying-structure-and-function-brain-hub-1118",
      "https://web.archive.org/web/20201118212800/https://news.mit.edu/2020/advancing-artificial-intelligence-research-1118",
      "https://web.archive.org/web/20201118215231/https://news.mit.edu/2020/smart-researchers-develop-gelatin-microcarrier-cell-production-1117",
      "https://web.archive.org/web/20201118180137/https://news.mit.edu/2020/chalk-radio-shares-mits-teaching-techniques-1117",
      "https://web.archive.org/web/20201118200420/https://news.mit.edu/2020/stem-week-event-encourages-students-see-themselves-science-technology-careers-1117",
      "https://web.archive.org/web/20201118180530/https://news.mit.edu/2020/mitnano-immersion-lab-gaming-program-awards-seed-grants-1117",
      "https://web.archive.org/web/20201119170536/https://news.mit.edu/2020/understanding-how-people-make-sense-information-manon-revel-1116",
      "https://web.archive.org/web/20201117170545/https://news.mit.edu/2020/3-questions-hsin-yu-chen-treading-lightly-when-dating-universe-1113",
      "https://web.archive.org/web/20201118180403/https://news.mit.edu/2020/j-pal-north-america-announces-housing-stability-evaluation-incubator-partners-1116",
      "https://web.archive.org/web/20201119181538/https://news.mit.edu/2020/3-questions-john-van-reenen-impact-technology-health-care-workers-1113",
      "https://web.archive.org/web/20201118174805/https://news.mit.edu/2020/oklo-nuclear-energy-1113",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(
        document.querySelector(".paragraph.paragraph--type--content-block-text")
      );
    },
  },
  [DOMAIN.EBB_MAGAZINE]: {
    getUrls: () => [
      "https://web.archive.org/web/20200529165203/https://www.ebb-magazine.com/essays/progressive-international",
      "https://web.archive.org/web/20200601235845/https://www.ebb-magazine.com/essays/studytube-and-the-fetishisation-of-productivity",
      "https://web.archive.org/web/20200506151206/https://www.ebb-magazine.com/essays/postscript-to-capitalist-realism",
      "https://web.archive.org/web/20200509124723/https://www.ebb-magazine.com/essays/lessons-from-lulzsec",
      "https://web.archive.org/web/20200520155245/https://www.ebb-magazine.com/essays/when-workers-shot-back",
      "https://web.archive.org/web/20200506150925/https://www.ebb-magazine.com/essays/bolsheviks-without-soviets",
      "https://web.archive.org/web/20200527112822/https://www.ebb-magazine.com/essays/social-democracy-and-its-discontents",
      "https://web.archive.org/web/20201110155501/https://www.ebb-magazine.com/essays/the-false-hope-of-a-biden-presidency",
      "https://web.archive.org/web/20201110155732/https://www.ebb-magazine.com/essays/imperialism-colombias-massacres-and-what-you-can-do-about-it",
      "https://web.archive.org/web/20201110155518/https://www.ebb-magazine.com/essays/the-force-of-nonviolence",
      "https://web.archive.org/web/20201110155702/https://www.ebb-magazine.com/essays/tracksuits-traumas-and-class-traitors",
      "https://web.archive.org/web/20201110155622/https://www.ebb-magazine.com/essays/against-performativity",
      "https://web.archive.org/web/20201110155530/https://www.ebb-magazine.com/essays/pandemic-reveals-the-necessity-of-revolutionary-socialism",
      "https://web.archive.org/web/20201110155610/https://www.ebb-magazine.com/essays/this-is-not-an-aberration",
      "https://web.archive.org/web/20201110155533/https://www.ebb-magazine.com/essays/the-jakarta-method",
      "https://web.archive.org/web/20201110155720/https://www.ebb-magazine.com/essays/keir-starmer-and-britains-road-to-socialism",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const banner = document.querySelector(".sqs-cookie-banner-v2");
      if (banner) {
        banner.remove();
      }

      const textNodes = Array.from(
        document.querySelectorAll(".sqs-block-content > p")
      )
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.FULLFACT]: {
    getUrls: () => [
      "https://web.archive.org/web/20201105181525/https://fullfact.org/health/coronavirus-lockdown-childcare/",
      "https://web.archive.org/web/20201105184625/https://fullfact.org/health/coronavirus-transmission-schools-claims/",
      "https://web.archive.org/web/20201106132437/https://fullfact.org/online/covid-test-not-vaccine/",
      "https://web.archive.org/web/20201108035626/https://fullfact.org/online/did-she-die-in-vain/",
      "https://web.archive.org/web/20201102151415/https://fullfact.org/online/bbc-presenters-did-remove-their-poppies-they-were-wearing-them-earlier-bbc-guidelines-allow/",
      "https://web.archive.org/web/20201106132352/https://fullfact.org/europe/brexit-support-poll/",
      "https://web.archive.org/web/20201106112016/https://fullfact.org/health/uk-column-covid-deaths/",
      "https://web.archive.org/web/20201101044827/https://fullfact.org/economy/lee-anderson-benefit-claim/",
      "https://web.archive.org/web/20201106112020/https://fullfact.org/online/2020-death-toll-comparison/",
      "https://web.archive.org/web/20201106010031/https://fullfact.org/online/vaccine-informed-consent/",
      "https://web.archive.org/web/20201104125711/https://fullfact.org/economy/japan-soy-sauce-bake-off/",
      "https://web.archive.org/web/20201101052606/https://fullfact.org/health/doctor-nhs-increase/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    getScrollOffsetHeight: () => {
      const elem = document.querySelector(".navbar");
      if (elem) {
        return elem.clientHeight;
      }
      return 0;
    },
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      const banner = document.querySelector(".alert");
      if (banner) {
        banner.remove();
      }

      const textNodes = Array.from(document.querySelectorAll("article > p"))
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.POPULA]: {
    getUrls: () => [
      "https://web.archive.org/web/20201113113915/https://popula.com/2020/11/13/for-shame/",
      "https://web.archive.org/web/20200924180250/https://popula.com/2018/10/10/one-of-yall-can-write-it-down/",
      "https://web.archive.org/web/20201003234945/https://popula.com/2020/09/16/abroad-the-distance-between-calcutta-and-california/",
      "https://web.archive.org/web/20201101123017/https://popula.com/2018/09/25/good-bones/",
      "https://web.archive.org/web/20201109004613/https://popula.com/2019/08/19/the-case-for-climate-rage/",
      "https://web.archive.org/web/20201101060423/https://popula.com/2019/11/12/methods-for-droplet-vitrification-on-the-eve-of-disaster/",
      "https://web.archive.org/web/20201112160636/https://popula.com/2020/11/12/my-three-lives-in-delhi/",
      "https://web.archive.org/web/20201111132856/https://popula.com/2020/11/11/to-feel-right/",
      "https://web.archive.org/web/20201108225121/https://popula.com/2020/11/08/popula-film-club-the-horror/",
      "https://web.archive.org/web/20201106220652/https://popula.com/2020/11/06/i-headed-to-get-the-flu-vaccine-with-a-spring-in-my-step/",
      "https://web.archive.org/web/20201106164041/https://popula.com/2020/11/06/endsars-and-the-nigerian-government/",
      "https://web.archive.org/web/20201119053941/https://popula.com/2020/11/03/how-ya-holding-up/",
      "https://web.archive.org/web/20201118115730/https://popula.com/2020/11/03/watch-out-the-worlds-behind-you/",
      "https://web.archive.org/web/20201102150615/https://popula.com/2020/11/02/perp-walk/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }
      const textNodes = Array.from(document.querySelectorAll("#pico > p"))
        .map(scope.getTextNodes)
        //@ts-ignore: this should work fine
        .flat();

      return textNodes;
    },
  },
  [DOMAIN.SEMIENGINEERING]: {
    getUrls: () => [
      "https://web.archive.org/web/20201031045633/https://semiengineering.com/a-renaissance-for-semiconductors/",
      "https://web.archive.org/web/20201101003551/https://semiengineering.com/much-smarter-manufacturing/",
      "https://web.archive.org/web/20201101022040/https://semiengineering.com/using-verification-data-more-effectively/",
      "https://web.archive.org/web/20201031200935/https://semiengineering.com/mask-lithography-issues-for-mature-nodes/",
      "https://web.archive.org/web/20201031234522/https://semiengineering.com/is-hardware-assisted-verification-avoidable/",
      "https://web.archive.org/web/20201028043036/https://semiengineering.com/making-chips-to-last-their-lifetime/",
      "https://web.archive.org/web/20201031120419/https://semiengineering.com/are-todays-mems-gyros-good-enough/",
      "https://web.archive.org/web/20201101010543/https://semiengineering.com/increase-in-analog-problems/",
      "https://web.archive.org/web/20201029032531/https://semiengineering.com/performance-and-power-tradeoffs-at-7-5nm/",
      "https://web.archive.org/web/20201101143839/https://semiengineering.com/slower-metal-bogs-down-soc-performance/",
      "https://web.archive.org/web/20201101064120/https://semiengineering.com/defect-challenges-grow-for-ic-packaging/",
      "https://web.archive.org/web/20201031225302/https://semiengineering.com/searching-for-power-bugs/",
    ],
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
    getBoundaryAncestorSelector: () => "p",
    parse: (scope: InjectScope): Text[] => {
      // preamble
      const wmHeader = document.getElementById("wm-ipp-base");
      if (wmHeader) {
        wmHeader.remove();
      }
      document.querySelectorAll("input, textarea").forEach((elem) => {
        elem.setAttribute("disabled", "disabled");
      });
      document.querySelectorAll("a").forEach((el) => {
        el.removeAttribute("href");
      });
      const viewportContent =
        "width=device-width, initial-scale=0.86, maximum-scale=0.86, minimum-scale=0.86";
      let viewport = document.querySelector("meta[name='viewport']");
      if (viewport) {
        viewport.setAttribute("content", viewportContent);
      } else {
        viewport = document.createElement("meta");
        viewport.setAttribute("name", "meta");
        viewport.setAttribute("content", viewportContent);
        document.head.appendChild(viewport);
      }

      return scope.getTextNodes(document.querySelector(".post_cnt"));
    },
  },
};
