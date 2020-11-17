import { DOMAIN, InjectScope, ParserInterface } from "./types";

export const parsers: { [domain: string]: ParserInterface } = {
  [DOMAIN.WIKIPEDIA]: {
    getUrl: () => {
      const urls = [
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
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24753564",
        "https://web.archive.org/web/20201013004139/https://news.ycombinator.com/item?id=24747667",
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
    getUrl: () => {
      const urls = [
        "https://web.archive.org/web/20201110155804/http://www.gutenberg.org/files/2701/2701-h/2701-h.htm",
        "https://web.archive.org/web/20201109153710/http://www.gutenberg.org/files/1946/1946-h/1946-h.htm",
        "https://web.archive.org/web/20200731222303/http://www.gutenberg.org/files/42324/42324-h/42324-h.htm",
        "https://web.archive.org/web/20201022141836/http://www.gutenberg.org/files/42671/42671-h/42671-h.htm",
        "https://web.archive.org/web/20201106022415/https://www.gutenberg.org/files/25344/25344-h/25344-h.htm",
        "https://web.archive.org/web/20201110000023/https://gutenberg.org/files/11/11-h/11-h.htm",
        "https://web.archive.org/web/20201106232638/http://www.gutenberg.org/files/5200/5200-h/5200-h.htm",
        "https://web.archive.org/web/20200523202814/https://www.gutenberg.org/files/1080/1080-h/1080-h.htm",
        "https://web.archive.org/web/20201107062000/http://www.gutenberg.org/files/844/844-h/844-h.htm",
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
    getUrl: () => {
      const urls = [
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
    getUrl: () => {
      const urls = [
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

      return scope.getTextNodes(document.getElementById("articleFullText"));
    },
  },
  [DOMAIN.PAUL_GRAHAM_BLOG]: {
    getUrl: () => {
      const urls = [
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
    getUrl: () => {
      const urls = [
        "https://web.archive.org/web/20201030000827/https://www.sec.gov/Archives/edgar/data/1810806/000119312520227862/d908875ds1.htm",
        "https://web.archive.org/web/20201116220437/https://www.sec.gov/Archives/edgar/data/1559720/000119312520294801/d81668ds1.htm",
        "https://web.archive.org/web/20201030000827/https://www.sec.gov/Archives/edgar/data/1810806/000119312520227862/d908875ds1.htm",
        "https://web.archive.org/web/20201109224957/https://www.sec.gov/Archives/edgar/data/1477720/000119312520228462/d855753ds1.htm",
        "https://web.archive.org/web/20201108070128/https://www.sec.gov/Archives/edgar/data/1643269/000119312520227201/d821436ds1.htm",
        "https://web.archive.org/web/20201115111400/https://www.sec.gov/Archives/edgar/data/1800667/000119312520228195/d841831ds1.htm",
        "https://web.archive.org/web/20201116041505/https://www.sec.gov/Archives/edgar/data/1585521/000119312519083351/d642624ds1.htm",
        "https://web.archive.org/web/20201103002558/https://www.sec.gov/Archives/edgar/data/1477333/000119312519222176/d735023ds1.htm",
        "https://web.archive.org/web/20201108031228/https://www.sec.gov/Archives/edgar/data/1561550/000119312519227783/d745413ds1.htm",
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

      return (
        Array.from(document.querySelectorAll("center"))
          .map(scope.getTextNodes)
          //@ts-ignore: this should work fine
          .flat()
      );
    },
  },
  [DOMAIN.THE_DIFF_SUBSTACK]: {
    getUrl: () => {
      const urls = [
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
    getUrl: () => {
      const urls = [
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

      return scope.getTextNodes(document.querySelector(".gdl-page-item"));
    },
  },
  [DOMAIN.GITHUB_BLOG]: {
    getUrl: () => {
      const urls = [
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
    getUrl: () => {
      const urls = [
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

      return scope.getTextNodes(document.querySelector("#markdownBody"));
    },
  },
};
