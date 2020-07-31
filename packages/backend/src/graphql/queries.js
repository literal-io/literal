/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const getAnnotation = /* GraphQL */ `
  query GetAnnotation($creatorId: AWSURL!, $id: AWSURL!) {
    getAnnotation(creatorId: $creatorId, id: $id) {
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
          value
          format
          language
          processingLanguage
          textDirection
          accessibility
          purpose
          rights
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
        id
        format
        language
        processingLanguage
        textDirection
        type
        accessibility
        rights
      }
      creatorId
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
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        createdAt
        updatedAt
      }
      generator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        createdAt
        updatedAt
      }
    }
  }
`;
export const listAnnotations = /* GraphQL */ `
  query ListAnnotations(
    $creatorId: AWSURL
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotations(
      creatorId: $creatorId
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
        creatorId
        created
        generated
        modified
        motivation
        via
        canonical
      }
      nextToken
    }
  }
`;
export const getAnnotationCollection = /* GraphQL */ `
  query GetAnnotationCollection($creatorId: AWSURL!, $id: AWSURL!) {
    getAnnotationCollection(creatorId: $creatorId, id: $id) {
      context
      id
      type
      label
      total
      first {
        context
        id
        type
        startIndex
        creatorId
        created
        modified
      }
      last {
        context
        id
        type
        startIndex
        creatorId
        created
        modified
      }
      creatorId
      created
      modified
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        createdAt
        updatedAt
      }
    }
  }
`;
export const listAnnotationCollections = /* GraphQL */ `
  query ListAnnotationCollections(
    $creatorId: AWSURL
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationCollectionFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationCollections(
      creatorId: $creatorId
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
        creatorId
        created
        modified
      }
      nextToken
    }
  }
`;
export const getAnnotationPageItem = /* GraphQL */ `
  query GetAnnotationPageItem($creatorId: AWSURL!, $id: ID!) {
    getAnnotationPageItem(creatorId: $creatorId, id: $id) {
      id
      annotationPageId
      annotationId
      annotation {
        context
        type
        id
        creatorId
        created
        generated
        modified
        motivation
        via
        canonical
      }
      annotationPage {
        context
        id
        type
        startIndex
        creatorId
        created
        modified
      }
      creatorId
      created
      modified
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        createdAt
        updatedAt
      }
    }
  }
`;
export const listAnnotationPageItems = /* GraphQL */ `
  query ListAnnotationPageItems(
    $creatorId: AWSURL
    $id: ModelIDKeyConditionInput
    $filter: ModelAnnotationPageItemFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationPageItems(
      creatorId: $creatorId
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
        creatorId
        created
        modified
      }
      nextToken
    }
  }
`;
export const getAnnotationPage = /* GraphQL */ `
  query GetAnnotationPage($creatorId: AWSURL!, $id: AWSURL!) {
    getAnnotationPage(creatorId: $creatorId, id: $id) {
      context
      id
      type
      partOf {
        context
        id
        type
        label
        total
        creatorId
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
        creatorId
        created
        modified
      }
      prev {
        context
        id
        type
        startIndex
        creatorId
        created
        modified
      }
      startIndex
      creatorId
      created
      modified
      creator {
        id
        type
        name
        nickname
        email_sha1
        email
        homepage
        createdAt
        updatedAt
      }
    }
  }
`;
export const listAnnotationPages = /* GraphQL */ `
  query ListAnnotationPages(
    $creatorId: AWSURL
    $id: ModelStringKeyConditionInput
    $filter: ModelAnnotationPageFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAnnotationPages(
      creatorId: $creatorId
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
        creatorId
        created
        modified
      }
      nextToken
    }
  }
`;
export const listAgents = /* GraphQL */ `
  query ListAgents(
    $id: AWSURL
    $filter: ModelAgentFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listAgents(
      id: $id
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
        createdAt
        updatedAt
      }
      nextToken
    }
  }
`;
export const getAgent = /* GraphQL */ `
  query GetAgent($id: AWSURL!) {
    getAgent(id: $id) {
      id
      type
      name
      nickname
      email_sha1
      email
      homepage
      createdAt
      updatedAt
    }
  }
`;
