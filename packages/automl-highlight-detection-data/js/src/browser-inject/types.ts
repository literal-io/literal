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
  scrollToRange(range: Range, size: ViewportSize): void;
  getSelectionAnnotations(
    range: Range,
    size: ViewportSize
  ): SelectionAnnotation[];
}

export interface ParserInterface {
  parse(scope: InjectScope): Text[];
  getUrl(): string;
  getBoundaryAncestorSelector(): string;
  isUrlEqual(url1: string, url2: string): boolean;
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
