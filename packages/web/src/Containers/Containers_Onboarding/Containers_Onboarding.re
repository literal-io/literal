open Containers_Onboarding_GraphQL;

[@react.component]
let make = (~currentUser, ~children) => {
  let (onboardingMutation, _s, _f) =
    ApolloHooks.useMutation(OnboardingMutation.definition);

  let _ =
    React.useEffect0(() => {
      /** enforce desired ordering of onboarding highlights via createdAt timestamps */

      let createAnnotationInputs =
        Containers_Onboarding_GraphQL.makeOnboardingAnnotations(~currentUser);

      let createAgentInput = {
        let username = AwsAmplify.Auth.CurrentUserInfo.(currentUser->username);
        let email =
          AwsAmplify.Auth.CurrentUserInfo.(currentUser->attributes->email);

        Lib_GraphQL.makeHash(~digest="SHA-1", email)
        |> Js.Promise.then_(hashedEmail =>
             Js.Promise.resolve({
               "id": Constants.apiOrigin ++ "/agents/" ++ username,
               "email": Some([|Some(email)|]),
               "email_sha1": Some([|Some(hashedEmail)|]),
               "type": `PERSON,
               "username": username,
               "homepage": None,
               "name": None,
               "nickname": None,
             })
           );
      };

      let _ =
        Js.Promise.all2((
          createAgentInput,
          Js.Promise.all(createAnnotationInputs),
        ))
        |> Js.Promise.then_(((createAgentInput, createAnnotationInputs)) => {
             let variables =
               OnboardingMutation.makeVariables(
                 ~createAgentInput,
                 ~createAnnotationInput1=createAnnotationInputs[0],
                 ~createAnnotationInput2=createAnnotationInputs[1],
                 ~createAnnotationInput3=createAnnotationInputs[2],
                 ~createAnnotationInput4=createAnnotationInputs[3],
                 ~createAnnotationInput5=createAnnotationInputs[4],
                 ~createAnnotationInput6=createAnnotationInputs[5],
                 (),
               );
             let result = onboardingMutation(~variables, ());
             let _ =
               Lib_GraphQL_CreateAnnotationMutation.Apollo.updateCacheMany(
                 ~currentUser,
                 ~inputs=createAnnotationInputs->Belt.Array.reverse,
                 ~createAnnotationCollection=true,
                 (),
               );
             result;
           })
        |> Js.Promise.then_(((mutationResult, _)) => {
             switch (mutationResult) {
             | ApolloHooks.Mutation.Errors(errors) =>
               Error.(report(ApolloMutationError(errors)))
             | NoData => Error.(report(ApolloEmptyData))
             | Data(_) => ()
             };
             Js.Promise.resolve();
           });

      None;
    });

  children;
};
