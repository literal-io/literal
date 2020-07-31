/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createAnnotation = /* GraphQL */ `
  mutation CreateAnnotation($input: CreateAnnotationInput!) {
    createAnnotation(input: $input) {
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
      annotationCollections {
        context
        id
        type
        label
        total
        creatorId
        created
        modified
      }
      annotationPageItems {
        id
        annotationPageId
        annotationId
        creatorId
        created
        modified
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
