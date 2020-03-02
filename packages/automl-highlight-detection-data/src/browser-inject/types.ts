export enum DOMAIN {
  WIKIPEDIA = 'wikipedia'
}

export interface InjectScope {
  getTextNodes(el: HTMLElement): Text[]
  getRandomRange(textNodes: Text[]): Range
  scrollToRange(range: Range): void
  getSelectionAnnotations(range: Range): SelectionAnnotation[]
}

export interface ParserInterface {
  scope: InjectScope
}

export type SerializedScope = {[key: string]: string}
export type RehydratedScope = InjectScope & {parser(scope: InjectScope): Text[]}

export type SelectionAnnotation = {
  label: 'highlight' | 'highlight_edge',
  boundingBox: {
    xRelativeMin: number,
    yRelativeMin: number,
    xRelativeMax: number,
    yRelativeMax: number
  }
}
