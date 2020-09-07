/* eslint-disable */
// this is an auto generated file. This will be overwritten

export const onCreateAgent = /* GraphQL */ `
  subscription OnCreateAgent($username: String!) {
    onCreateAgent(username: $username) {
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
export const onUpdateAgent = /* GraphQL */ `
  subscription OnUpdateAgent($username: String!) {
    onUpdateAgent(username: $username) {
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
export const onDeleteAgent = /* GraphQL */ `
  subscription OnDeleteAgent($username: String!) {
    onDeleteAgent(username: $username) {
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
