/* eslint-disable */
// this is an auto generated file. This will be overwritten

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
      creatorUsername
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
        created
        creatorUsername
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
      created
      creatorUsername
      modified
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
      first {
        context
        id
        type
        startIndex
        created
        modified
        creatorUsername
      }
      last {
        context
        id
        type
        startIndex
        created
        modified
        creatorUsername
      }
    }
  }
`;
export const getAnnotationPageItem = /* GraphQL */ `
  query GetAnnotationPageItem($creatorUsername: String!, $id: ID!) {
    getAnnotationPageItem(creatorUsername: $creatorUsername, id: $id) {
      id
      annotationPageId
      annotationId
      created
      modified
      creatorUsername
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
        annotationPageId
        annotationId
        created
        modified
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
      creatorUsername
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
      partOf {
        context
        id
        type
        label
        total
        created
        creatorUsername
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
        creatorUsername
      }
      prev {
        context
        id
        type
        startIndex
        created
        modified
        creatorUsername
      }
    }
  }
`;
