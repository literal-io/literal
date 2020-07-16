/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createAnnotationCollection = /* GraphQL */ `
  mutation CreateAnnotationCollection(
    $input: CreateAnnotationCollectionInput!
    $condition: ModelAnnotationCollectionConditionInput
  ) {
    createAnnotationCollection(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      last {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const updateAnnotationCollection = /* GraphQL */ `
  mutation UpdateAnnotationCollection(
    $input: UpdateAnnotationCollectionInput!
    $condition: ModelAnnotationCollectionConditionInput
  ) {
    updateAnnotationCollection(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      last {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const deleteAnnotationCollection = /* GraphQL */ `
  mutation DeleteAnnotationCollection(
    $input: DeleteAnnotationCollectionInput!
    $condition: ModelAnnotationCollectionConditionInput
  ) {
    deleteAnnotationCollection(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      last {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const createAnnotationPageItem = /* GraphQL */ `
  mutation CreateAnnotationPageItem(
    $input: CreateAnnotationPageItemInput!
    $condition: ModelAnnotationPageItemConditionInput
  ) {
    createAnnotationPageItem(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const updateAnnotationPageItem = /* GraphQL */ `
  mutation UpdateAnnotationPageItem(
    $input: UpdateAnnotationPageItemInput!
    $condition: ModelAnnotationPageItemConditionInput
  ) {
    updateAnnotationPageItem(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const deleteAnnotationPageItem = /* GraphQL */ `
  mutation DeleteAnnotationPageItem(
    $input: DeleteAnnotationPageItemInput!
    $condition: ModelAnnotationPageItemConditionInput
  ) {
    deleteAnnotationPageItem(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
      }
      creatorId
      created
      createdAt
      updatedAt
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
export const createAnnotationPage = /* GraphQL */ `
  mutation CreateAnnotationPage(
    $input: CreateAnnotationPageInput!
    $condition: ModelAnnotationPageConditionInput
  ) {
    createAnnotationPage(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
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
        createdAt
        updatedAt
      }
      prev {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      startIndex
      creatorId
      created
      createdAt
      updatedAt
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
export const updateAnnotationPage = /* GraphQL */ `
  mutation UpdateAnnotationPage(
    $input: UpdateAnnotationPageInput!
    $condition: ModelAnnotationPageConditionInput
  ) {
    updateAnnotationPage(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
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
        createdAt
        updatedAt
      }
      prev {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      startIndex
      creatorId
      created
      createdAt
      updatedAt
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
export const deleteAnnotationPage = /* GraphQL */ `
  mutation DeleteAnnotationPage(
    $input: DeleteAnnotationPageInput!
    $condition: ModelAnnotationPageConditionInput
  ) {
    deleteAnnotationPage(input: $input, condition: $condition) {
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
        createdAt
        updatedAt
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
        createdAt
        updatedAt
      }
      prev {
        context
        id
        type
        startIndex
        creatorId
        created
        createdAt
        updatedAt
      }
      startIndex
      creatorId
      created
      createdAt
      updatedAt
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
export const createAgent = /* GraphQL */ `
  mutation CreateAgent(
    $input: CreateAgentInput!
    $condition: ModelAgentConditionInput
  ) {
    createAgent(input: $input, condition: $condition) {
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
export const updateAgent = /* GraphQL */ `
  mutation UpdateAgent(
    $input: UpdateAgentInput!
    $condition: ModelAgentConditionInput
  ) {
    updateAgent(input: $input, condition: $condition) {
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
export const deleteAgent = /* GraphQL */ `
  mutation DeleteAgent(
    $input: DeleteAgentInput!
    $condition: ModelAgentConditionInput
  ) {
    deleteAgent(input: $input, condition: $condition) {
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
