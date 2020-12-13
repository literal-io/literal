/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const annotationCollectionLabelAutocomplete = /* GraphQL */ `
  query AnnotationCollectionLabelAutocomplete(
    $input: AnnotationCollectionLabelAutocompleteInput!
  ) {
    annotationCollectionLabelAutocomplete(input: $input) {
      items {
        context
        id
        type
        label
        total
        firstAnnotationPageId
        lastAnnotationPageId
        creatorUsername
        created
        modified
      }
      nextToken
    }
  }
`;
export const getAgent = /* GraphQL */ `
  query GetAgent($username: String!) {
    getAgent(username: $username) {
      id
      type
      name
      nickname
      email_sha1
      email
      homepage
      username
      created
      modified
    }
  }
`;
export const listAgents = /* GraphQL */ `
  query ListAgents(
    $username: String
    $filter: ModelAgentFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAgents(
      username: $username
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      nextToken
    }
  }
`;
export const listAnnotations = /* GraphQL */ `
  query ListAnnotations(
    $creatorUsername: String
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotations(
      creatorUsername: $creatorUsername
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        context
        type
        id
        created
        generated
        modified
        motivation
        via
        canonical
        creatorUsername
      }
      nextToken
    }
  }
`;
export const getAnnotation = /* GraphQL */ `
  query GetAnnotation($creatorUsername: String!, $id: String!) {
    getAnnotation(creatorUsername: $creatorUsername, id: $id) {
      context
      type
      id
      body {
        ... on ExternalBody {
          id
          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
          type
        }
        ... on TextualBody {
          id
          format
          language
          processingLanguage
          textDirection
          accessibility
          purpose
          rights
          value
          type
        }
        ... on ChoiceBody {
          type
        }
        ... on SpecificBody {
          id
          type
          purpose
          styleClass
          scope
        }
      }
      target {
        ... on ExternalTarget {
          id
          format
          language
          processingLanguage
          textDirection
          type
          accessibility
          rights
        }
        ... on TextualTarget {
          id
          format
          language
          processingLanguage
          textDirection
          accessibility
          rights
          value
        }
      }
      created
      generator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      generated
      modified
      audience {
        id
        type
      }
      motivation
      via
      canonical
      stylesheet {
        type
        value
      }
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      creatorUsername
    }
  }
`;
export const listAnnotationCollections = /* GraphQL */ `
  query ListAnnotationCollections(
    $creatorUsername: String
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationCollectionFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationCollections(
      creatorUsername: $creatorUsername
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        context
        id
        type
        label
        total
        firstAnnotationPageId
        lastAnnotationPageId
        creatorUsername
        created
        modified
      }
      nextToken
    }
  }
`;
export const getAnnotationCollection = /* GraphQL */ `
  query GetAnnotationCollection($creatorUsername: String!, $id: String!) {
    getAnnotationCollection(creatorUsername: $creatorUsername, id: $id) {
      context
      id
      type
      label
      total
      firstAnnotationPageId
      last {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
      lastAnnotationPageId
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      creatorUsername
      created
      modified
      first {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
    }
  }
`;
export const listAnnotationCollectionsByLabel = /* GraphQL */ `
  query ListAnnotationCollectionsByLabel(
    $creatorUsername: String
    $label: ModelStringKeyConditionInput
    $sortDirection: ModelSortDirection
    $filter: ModelAnnotationCollectionFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listAnnotationCollectionsByLabel(
      creatorUsername: $creatorUsername
      label: $label
      sortDirection: $sortDirection
      filter: $filter
      limit: $limit
      nextToken: $nextToken
    ) {
      items {
        context
        id
        type
        label
        total
        firstAnnotationPageId
        lastAnnotationPageId
        creatorUsername
        created
        modified
      }
      nextToken
    }
  }
`;
export const getAnnotationPageItem = /* GraphQL */ `
  query GetAnnotationPageItem($creatorUsername: String!, $id: ID!) {
    getAnnotationPageItem(creatorUsername: $creatorUsername, id: $id) {
      id
      created
      modified
      annotationId
      annotationPageId
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      creatorUsername
      annotation {
        context
        type
        id
        created
        generated
        modified
        motivation
        via
        canonical
        creatorUsername
      }
      annotationPage {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
    }
  }
`;
export const listAnnotationPageItems = /* GraphQL */ `
  query ListAnnotationPageItems(
    $creatorUsername: String
    $id: ModelIDKeyConditionInput
    $filter: ModelAnnotationPageItemFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationPageItems(
      creatorUsername: $creatorUsername
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        id
        created
        modified
        annotationId
        annotationPageId
        creatorUsername
      }
      nextToken
    }
  }
`;
export const listAnnotationPages = /* GraphQL */ `
  query ListAnnotationPages(
    $creatorUsername: String
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationPageFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationPages(
      creatorUsername: $creatorUsername
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
      nextToken
    }
  }
`;
export const getAnnotationPage = /* GraphQL */ `
  query GetAnnotationPage($creatorUsername: String!, $id: String!) {
    getAnnotationPage(creatorUsername: $creatorUsername, id: $id) {
      context
      id
      type
      startIndex
      created
      modified
      partOfAnnotationCollectionId
      nextAnnotationPageId
      prevAnnotationPageId
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        username
        created
        modified
      }
      creatorUsername
      partOf {
        context
        id
        type
        label
        total
        firstAnnotationPageId
        lastAnnotationPageId
        creatorUsername
        created
        modified
      }
      items {
        nextToken
      }
      next {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
      prev {
        context
        id
        type
        startIndex
        created
        modified
        partOfAnnotationCollectionId
        nextAnnotationPageId
        prevAnnotationPageId
        creatorUsername
      }
    }
  }
`;
