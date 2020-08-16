/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createAnnotation = /* GraphQL */ `
  mutation CreateAnnotation($input: CreateAnnotationInput!) {
    createAnnotation(input: $input) {
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
      annotationCollections {
        context
        id
        type
        label
        total
        created
        creatorUsername
        modified
      }
      annotationPageItems {
        id
        annotationPageId
        annotationId
        created
        modified
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
export const updateAnnotation = /* GraphQL */ `
  mutation UpdateAnnotation($input: UpdateAnnotationInput!) {
    updateAnnotation(input: $input) {
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
      annotationCollections {
        context
        id
        type
        label
        total
        created
        creatorUsername
        modified
      }
      annotationPageItems {
        id
        annotationPageId
        annotationId
        created
        modified
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
      username
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
      username
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
      username
      createdAt
      updatedAt
    }
  }
`;
