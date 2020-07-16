/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateAnnotationCollection = /* GraphQL */ `
  subscription OnCreateAnnotationCollection($creatorId: String!) {
    onCreateAnnotationCollection(creatorId: $creatorId) {
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
export const onUpdateAnnotationCollection = /* GraphQL */ `
  subscription OnUpdateAnnotationCollection($creatorId: String!) {
    onUpdateAnnotationCollection(creatorId: $creatorId) {
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
export const onDeleteAnnotationCollection = /* GraphQL */ `
  subscription OnDeleteAnnotationCollection($creatorId: String!) {
    onDeleteAnnotationCollection(creatorId: $creatorId) {
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
export const onCreateAnnotationPageItem = /* GraphQL */ `
  subscription OnCreateAnnotationPageItem($creatorId: String!) {
    onCreateAnnotationPageItem(creatorId: $creatorId) {
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
export const onUpdateAnnotationPageItem = /* GraphQL */ `
  subscription OnUpdateAnnotationPageItem($creatorId: String!) {
    onUpdateAnnotationPageItem(creatorId: $creatorId) {
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
export const onDeleteAnnotationPageItem = /* GraphQL */ `
  subscription OnDeleteAnnotationPageItem($creatorId: String!) {
    onDeleteAnnotationPageItem(creatorId: $creatorId) {
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
export const onCreateAnnotationPage = /* GraphQL */ `
  subscription OnCreateAnnotationPage($creatorId: String!) {
    onCreateAnnotationPage(creatorId: $creatorId) {
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
export const onUpdateAnnotationPage = /* GraphQL */ `
  subscription OnUpdateAnnotationPage($creatorId: String!) {
    onUpdateAnnotationPage(creatorId: $creatorId) {
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
export const onDeleteAnnotationPage = /* GraphQL */ `
  subscription OnDeleteAnnotationPage($creatorId: String!) {
    onDeleteAnnotationPage(creatorId: $creatorId) {
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
export const onCreateAgent = /* GraphQL */ `
  subscription OnCreateAgent($id: String!) {
    onCreateAgent(id: $id) {
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
export const onUpdateAgent = /* GraphQL */ `
  subscription OnUpdateAgent($id: String!) {
    onUpdateAgent(id: $id) {
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
export const onDeleteAgent = /* GraphQL */ `
  subscription OnDeleteAgent($id: String!) {
    onDeleteAgent(id: $id) {
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
