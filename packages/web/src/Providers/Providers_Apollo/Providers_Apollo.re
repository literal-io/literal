%raw
"require('isomorphic-fetch')";
let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

[@react.component]
let make = (~render) => {
  let Providers_Authentication.{user} =
    React.useContext(Providers_Authentication.authenticationContext);
  let previousUserRef = React.useRef(Providers_Authentication_User.Unknown);
  let (innerClient, setClient) =
    React.useState(_ => Providers_Apollo_Client.inst^);

  React.useEffect1(
    () => {
      let authHasChanged =
        switch (user, previousUserRef.current) {
        | (SignedInUser(_), Unknown)
        | (SignedInUser(_), GuestUser(_))
        | (SignedInUser(_), SignedOutPromptAuthentication)
        | (GuestUser(_), SignedInUser(_))
        | (GuestUser(_), Unknown)
        | (GuestUser(_), SignedInUserMergingIdentites(_))
        | (GuestUser(_), SignedOutPromptAuthentication)
        | (Unknown, SignedInUser(_))
        | (Unknown, GuestUser(_))
        | (Unknown, SignedInUserMergingIdentites(_))
        | (Unknown, SignedOutPromptAuthentication)
        | (SignedInUserMergingIdentites(_), Unknown)
        | (SignedInUserMergingIdentites(_), GuestUser(_)) => true
        | _ => false
        };
      let _ =
        if (authHasChanged) {
          let newClient = Providers_Apollo_Client.makeClientFromUser(user);
          setClient(_ => newClient);
          Providers_Apollo_Client.inst := newClient;
        };

      previousUserRef.current = user;

      None;
    },
    [|user|],
  );

  <ReasonApollo.Provider client=innerClient>
    <AwsAppSync.Rehydrated
      render={({rehydrated}) =>
        <ApolloHooks.Provider client=innerClient>
          {render(~rehydrated)}
        </ApolloHooks.Provider>
      }
    />
  </ReasonApollo.Provider>;
};
