import { DOMAIN, InjectScope } from './types'

export const parsers = {
  [DOMAIN.WIKIPEDIA]: (scope: InjectScope): Text[] => {
    // open all closed sections
    document
      .querySelectorAll('#bodyContent .collapsible-heading,.collapsible-block')
      .forEach((elem) => {elem.classList.add("open-block")})

    const textNodes = scope.getTextNodes(document.querySelector('#bodyContent'))

    return textNodes
  }
}

