export enum DOMAIN {
  WIKIPEDIA = 'wikipedia'
}

export interface InjectScope {
  getTextNodes(el: HTMLElement): Text[]
  getRandomRange(textNodes: Text[]): Range
  scrollToRange(range: Range): void
}

export interface ParserInterface {
  scope: InjectScope
}

export type SerializedScope = {[key: string]: string}
export type RehydratedScope = InjectScope & {parser(scope: InjectScope): Text[]}

export type Injectable = {
  handler(scope: SerializedScope): void,
  args: SerializedScope
}
