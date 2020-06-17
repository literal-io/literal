/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateScreenshot = /* GraphQL */ `
  subscription OnCreateScreenshot($owner: String!) {
    onCreateScreenshot(owner: $owner) {
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
export const onUpdateScreenshot = /* GraphQL */ `
  subscription OnUpdateScreenshot {
    onUpdateScreenshot {
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
export const onDeleteScreenshot = /* GraphQL */ `
  subscription OnDeleteScreenshot {
    onDeleteScreenshot {
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
export const onCreateTag = /* GraphQL */ `
  subscription OnCreateTag($owner: String!) {
    onCreateTag(owner: $owner) {
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
export const onUpdateTag = /* GraphQL */ `
  subscription OnUpdateTag($owner: String!) {
    onUpdateTag(owner: $owner) {
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
export const onDeleteTag = /* GraphQL */ `
  subscription OnDeleteTag($owner: String!) {
    onDeleteTag(owner: $owner) {
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
export const onCreateHighlight = /* GraphQL */ `
  subscription OnCreateHighlight($owner: String!) {
    onCreateHighlight(owner: $owner) {
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
export const onUpdateHighlight = /* GraphQL */ `
  subscription OnUpdateHighlight($owner: String!) {
    onUpdateHighlight(owner: $owner) {
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
export const onDeleteHighlight = /* GraphQL */ `
  subscription OnDeleteHighlight($owner: String!) {
    onDeleteHighlight(owner: $owner) {
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
export const onCreateHighlightTag = /* GraphQL */ `
  subscription OnCreateHighlightTag($owner: String!) {
    onCreateHighlightTag(owner: $owner) {
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
export const onUpdateHighlightTag = /* GraphQL */ `
  subscription OnUpdateHighlightTag($owner: String!) {
    onUpdateHighlightTag(owner: $owner) {
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
export const onDeleteHighlightTag = /* GraphQL */ `
  subscription OnDeleteHighlightTag($owner: String!) {
    onDeleteHighlightTag(owner: $owner) {
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
export const onCreateProfile = /* GraphQL */ `
  subscription OnCreateProfile {
    onCreateProfile {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
export const onUpdateProfile = /* GraphQL */ `
  subscription OnUpdateProfile($owner: String!) {
    onUpdateProfile(owner: $owner) {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
export const onDeleteProfile = /* GraphQL */ `
  subscription OnDeleteProfile {
    onDeleteProfile {
      id
      owner
      createdAt
      isOnboarded
    }
  }
`;
