let signInWithToken = [%raw {|
  function signInWithToken (idToken, refreshToken, accessToken) {
    console.log("signInWithToken", { idToken, refreshToken, accessToken })
    const { 
      CognitoUserSession,
      CognitoIdToken,
      CognitoRefreshToken,
      CognitoAccessToken
    } = require('amazon-cognito-identity-js')
    const { Credentials, Hub } = require('@aws-amplify/core')
    const { default: Auth } = require('@aws-amplify/auth')

    const session = new CognitoUserSession({
      IdToken: new CognitoIdToken({ IdToken: idToken }),
      RefreshToken: new CognitoRefreshToken({ RefreshToken: refreshToken }),
      AccessToken: new CognitoAccessToken({ AccessToken: accessToken })
    })

    return Credentials.set(session, 'session')
      .then((credentials) => {
        const currentUser = 
          Auth.createCognitoUser(
            session.getIdToken().decodePayload()['cognito:username']
          )
        currentUser.setSignInUserSession(session)
      })
  }
|}];
