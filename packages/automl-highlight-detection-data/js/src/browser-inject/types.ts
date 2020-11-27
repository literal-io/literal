export enum DOMAIN {
  WIKIPEDIA = "wikipedia",
  HACKERNEWS = "hackernews",
  RIBBONFARM = "ribbonfarm",
  PROJECT_GUTENBERG = "project-gutenberg",
  ACOUP = "acoup",
  ACM_BLOG = "acm-blog",
  PAUL_GRAHAM_BLOG = "paul-graham-blog",
  SEC = "sec",
  THE_DIFF_SUBSTACK = "the-diff-substack",
  PETER_TURCHIN_BLOG = "peter-turchin-blog",
  GITHUB_BLOG = "github-blog",
  GWERN_BLOG = "gwern-blog",
  NATURE = "nature",
  TONSKY_BLOG = "tonksy-blog",
  FIRST_ROUND_BLOG = "first-round-blog",
  CRYTOGRAPHY_ENGINEERING_BLOG = "cryptography-engineering-blog",
  PARIS_REVIEW = "paris-review",
  COCKROACH_DB_BLOG = "cockroach-db-blog",
  LAPHAMS_QUARTERLY = "laphams-quarterly",
  GOOGLE_AI_BLOG = "google-ai-blog",
  NPR_TEXT = "npr-text",
  BOOK_FORUM = "book-forum",
  NVIDIA_NEWS = "nvidia-news",
  GEMSBOK_BLOG = "gemsbok-blog",
  THE_MARKUP = "the-markup",
  STANFORD_NEWS = "stanford-news",
  MIT_NEWS = "mit-news",
  EBB_MAGAZINE = "ebb-magazine",
  FULLFACT = "full-fact",
  POPULA = "popula",
  SEMIENGINEERING = "semiengineering",
}

export type ViewportSize = { width: number; height: number; scale: number };

export interface InjectScope {
  getViewportSize(): ViewportSize;
  getTextNodes(el: HTMLElement): Text[];
  getRandomRange(
    textNodes: Text[],
    boundaryAncestorSelector: string,
    size: ViewportSize
  ): Range;
  styleRange(range: Range): void;
  scrollToRange(range: Range, size: ViewportSize, offsetHeight: number): void;
  getSelectionAnnotations(
    range: Range,
    size: ViewportSize
  ): SelectionAnnotation[];
}

export interface ParserInterface {
  parse(scope: InjectScope): Text[];
  getUrls(): string[];
  getBoundaryAncestorSelector(): string;
  isUrlEqual(url1: string, url2: string): boolean;
  getScrollOffsetHeight?: () => number;
}

export type SerializedScope = { [key: string]: string };
export type RehydratedScope = InjectScope & ParserInterface;

export type SelectionAnnotation = {
  label: "highlight" | "highlight_edge";
  boundingBox: {
    xRelativeMin: number;
    yRelativeMin: number;
    xRelativeMax: number;
    yRelativeMax: number;
  };
};
