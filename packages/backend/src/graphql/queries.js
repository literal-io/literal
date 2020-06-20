/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const listScreenshots = /* GraphQL */ `
  query ListScreenshots(
    $filter: ModelScreenshotFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listScreenshots(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        createdAt
        owner
      }
      nextToken
    }
  }
`;
export const getScreenshot = /* GraphQL */ `
  query GetScreenshot($id: ID!) {
    getScreenshot(id: $id) {
      id
      createdAt
      file {
        bucket
        key
        region
      }
      owner
    }
  }
`;
export const listTags = /* GraphQL */ `
  query ListTags(
    $id: ID
    $filter: ModelTagFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listTags(
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        id
        createdAt
        text
        owner
      }
      nextToken
    }
  }
`;
export const getTag = /* GraphQL */ `
  query GetTag($id: ID!) {
    getTag(id: $id) {
      id
      createdAt
      text
      owner
      highlights {
        nextToken
      }
    }
  }
`;
export const listHighlights = /* GraphQL */ `
  query ListHighlights(
    $id: ID
    $filter: ModelHighlightFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listHighlights(
      id: $id
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        id
        createdAt
        text
        note
        owner
      }
      nextToken
    }
  }
`;
export const getHighlight = /* GraphQL */ `
  query GetHighlight($id: ID!) {
    getHighlight(id: $id) {
      id
      createdAt
      text
      note
      owner
      screenshot {
        id
        createdAt
        owner
      }
      tags {
        nextToken
      }
    }
  }
`;
export const getHighlightTag = /* GraphQL */ `
  query GetHighlightTag($id: ID!) {
    getHighlightTag(id: $id) {
      id
      highlightId
      tagId
      createdAt
      tag {
        id
        createdAt
        text
        owner
      }
      highlight {
        id
        createdAt
        text
        note
        owner
      }
    }
  }
`;
export const listHighlightTags = /* GraphQL */ `
  query ListHighlightTags(
    $filter: ModelHighlightTagFilterInput
    $limit: Int
    $nextToken: String
  ) {
    listHighlightTags(filter: $filter, limit: $limit, nextToken: $nextToken) {
      items {
        id
        highlightId
        tagId
        createdAt
      }
      nextToken
    }
  }
`;
export const getProfile = /* GraphQL */ `
  query GetProfile($owner: String!) {
    getProfile(owner: $owner) {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
export const listProfiles = /* GraphQL */ `
  query ListProfiles(
    $owner: String
    $filter: ModelProfileFilterInput
    $limit: Int
    $nextToken: String
    $sortDirection: ModelSortDirection
  ) {
    listProfiles(
      owner: $owner
      filter: $filter
      limit: $limit
      nextToken: $nextToken
      sortDirection: $sortDirection
    ) {
      items {
        id
        owner
        createdAt
        isOnboarded
      }
      nextToken
    }
  }
`;
