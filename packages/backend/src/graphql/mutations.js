/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const createHighlightFromScreenshot = /* GraphQL */ `
  mutation CreateHighlightFromScreenshot(
    $input: CreateHighlightFromScreenshotInput!
  ) {
    createHighlightFromScreenshot(input: $input) {
      id
      createdAt
      text
      note
      screenshot {
        id
        createdAt
        owner
      }
      owner
      tags {
        nextToken
      }
    }
  }
`;
export const createTags = /* GraphQL */ `
  mutation CreateTags($input: [CreateTagInput!]!) {
    createTags(input: $input) {
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
export const createHighlightTags = /* GraphQL */ `
  mutation CreateHighlightTags($input: [CreateHighlightTagInput!]!) {
    createHighlightTags(input: $input) {
      id
      highlightId
      tagId
      createdAt
      highlight {
        id
        createdAt
        text
        note
        owner
      }
      tag {
        id
        createdAt
        text
        owner
      }
    }
  }
`;
export const deleteHighlightTags = /* GraphQL */ `
  mutation DeleteHighlightTags($input: [DeleteHighlightTagInput!]!) {
    deleteHighlightTags(input: $input) {
      id
      highlightId
      tagId
      createdAt
      highlight {
        id
        createdAt
        text
        note
        owner
      }
      tag {
        id
        createdAt
        text
        owner
      }
    }
  }
`;
export const updateHighlightAndTags = /* GraphQL */ `
  mutation UpdateHighlightAndTags($input: UpdateHighlightAndTagsInput!) {
    updateHighlightAndTags(input: $input) {
      updateHighlight {
        id
        createdAt
        text
        note
        owner
      }
      createTags {
        id
        createdAt
        text
        owner
      }
      createHighlightTags {
        id
        highlightId
        tagId
        createdAt
      }
      deleteHighlightTags {
        id
      }
    }
  }
`;
export const createScreenshot = /* GraphQL */ `
  mutation CreateScreenshot(
    $input: CreateScreenshotInput!
    $condition: ModelScreenshotConditionInput
  ) {
    createScreenshot(input: $input, condition: $condition) {
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
export const updateScreenshot = /* GraphQL */ `
  mutation UpdateScreenshot(
    $input: UpdateScreenshotInput!
    $condition: ModelScreenshotConditionInput
  ) {
    updateScreenshot(input: $input, condition: $condition) {
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
export const deleteScreenshot = /* GraphQL */ `
  mutation DeleteScreenshot(
    $input: DeleteScreenshotInput!
    $condition: ModelScreenshotConditionInput
  ) {
    deleteScreenshot(input: $input, condition: $condition) {
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
export const createTag = /* GraphQL */ `
  mutation CreateTag(
    $input: CreateTagInput!
    $condition: ModelTagConditionInput
  ) {
    createTag(input: $input, condition: $condition) {
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
export const updateTag = /* GraphQL */ `
  mutation UpdateTag(
    $input: UpdateTagInput!
    $condition: ModelTagConditionInput
  ) {
    updateTag(input: $input, condition: $condition) {
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
export const deleteTag = /* GraphQL */ `
  mutation DeleteTag(
    $input: DeleteTagInput!
    $condition: ModelTagConditionInput
  ) {
    deleteTag(input: $input, condition: $condition) {
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
export const createHighlight = /* GraphQL */ `
  mutation CreateHighlight(
    $input: CreateHighlightInput!
    $condition: ModelHighlightConditionInput
  ) {
    createHighlight(input: $input, condition: $condition) {
      id
      createdAt
      text
      note
      screenshot {
        id
        createdAt
        owner
      }
      owner
      tags {
        nextToken
      }
    }
  }
`;
export const updateHighlight = /* GraphQL */ `
  mutation UpdateHighlight(
    $input: UpdateHighlightInput!
    $condition: ModelHighlightConditionInput
  ) {
    updateHighlight(input: $input, condition: $condition) {
      id
      createdAt
      text
      note
      screenshot {
        id
        createdAt
        owner
      }
      owner
      tags {
        nextToken
      }
    }
  }
`;
export const deleteHighlight = /* GraphQL */ `
  mutation DeleteHighlight(
    $input: DeleteHighlightInput!
    $condition: ModelHighlightConditionInput
  ) {
    deleteHighlight(input: $input, condition: $condition) {
      id
      createdAt
      text
      note
      screenshot {
        id
        createdAt
        owner
      }
      owner
      tags {
        nextToken
      }
    }
  }
`;
export const createHighlightTag = /* GraphQL */ `
  mutation CreateHighlightTag(
    $input: CreateHighlightTagInput!
    $condition: ModelHighlightTagConditionInput
  ) {
    createHighlightTag(input: $input, condition: $condition) {
      id
      highlightId
      tagId
      createdAt
      highlight {
        id
        createdAt
        text
        note
        owner
      }
      tag {
        id
        createdAt
        text
        owner
      }
    }
  }
`;
export const updateHighlightTag = /* GraphQL */ `
  mutation UpdateHighlightTag(
    $input: UpdateHighlightTagInput!
    $condition: ModelHighlightTagConditionInput
  ) {
    updateHighlightTag(input: $input, condition: $condition) {
      id
      highlightId
      tagId
      createdAt
      highlight {
        id
        createdAt
        text
        note
        owner
      }
      tag {
        id
        createdAt
        text
        owner
      }
    }
  }
`;
export const deleteHighlightTag = /* GraphQL */ `
  mutation DeleteHighlightTag(
    $input: DeleteHighlightTagInput!
    $condition: ModelHighlightTagConditionInput
  ) {
    deleteHighlightTag(input: $input, condition: $condition) {
      id
      highlightId
      tagId
      createdAt
      highlight {
        id
        createdAt
        text
        note
        owner
      }
      tag {
        id
        createdAt
        text
        owner
      }
    }
  }
`;
export const updateProfile = /* GraphQL */ `
  mutation UpdateProfile(
    $input: UpdateProfileInput!
    $condition: ModelProfileConditionInput
  ) {
    updateProfile(input: $input, condition: $condition) {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
export const deleteProfile = /* GraphQL */ `
  mutation DeleteProfile(
    $input: DeleteProfileInput!
    $condition: ModelProfileConditionInput
  ) {
    deleteProfile(input: $input, condition: $condition) {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
export const createProfile = /* GraphQL */ `
  mutation CreateProfile(
    $input: CreateProfileInput!
    $condition: ModelProfileConditionInput
  ) {
    createProfile(input: $input, condition: $condition) {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
