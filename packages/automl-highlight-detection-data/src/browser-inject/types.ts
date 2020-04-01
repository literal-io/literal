export enum DOMAIN {
  WIKIPEDIA = "wikipedia",
  HACKERNEWS = "hackernews",
  RIBBONFARM = "ribbonfarm",
}

export interface InjectScope {
  getTextNodes(el: HTMLElement): Text[];
  getRandomRange(textNodes: Text[], boundaryAncestorSelector: string): Range;
  scrollToRange(range: Range): void;
  getSelectionAnnotations(range: Range): SelectionAnnotation[];
}

export interface ParserInterface {
  parse(scope: InjectScope): Text[];
  getUrl(): string;
  getBoundaryAncestorSelector(): string;
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
