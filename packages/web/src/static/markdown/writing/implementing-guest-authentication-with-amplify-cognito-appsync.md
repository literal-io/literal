Literal supports "guest authentication", i.e. a new user can download the application and start using it right away, without needing to create an account first.

Guest authentication looks like this:

<blockquote class="twitter-tweet"><p lang="en" dir="ltr">literal now no longer requires sign up to use! <a href="https://t.co/1ytKM5DL6I">pic.twitter.com/1ytKM5DL6I</a></p>&mdash; literal (@literal_app) <a href="https://twitter.com/literal_app/status/1397301007214948357?ref_src=twsrc%5Etfw">May 25, 2021</a></blockquote> <script async src="https://platform.twitter.com/widgets.js" charset="utf-8"></script>

From a user's perspective, this is a subtle feature, as the outcome is a _lack of experience_: they aren't interrupted with an intrusive prompt to create an account. From a product perspective however, this functionality has a large impact in improving conversion in an onboarding funnel. Ideally, this is really how most products should work. It significantly reduces the friction involved in a new user trying your product in that it doesn't require that user to surrender their email to an application they've never used. 

In spite of the clear product advantages, developers often eschew support for guest authentication in favor of prompting for sign up. This pattern is particularly observable within web applications and mobile applications that aren't local-first; interactions with cloud services usually involve authentication of some form. I believe this situation stems partly due from business constraints (e.g. I may want to collect an email to ensure that I can deliver marketing communications), but also partly from the perceived complexity of implementing and maintaining such a system.

In this post, I want to address the latter by detailing how guest authentication is implemented within Literal, as well as illustrate how such a system is implemented with AWS Amplify, Cognito, and AppSync. The end result is a system in which a user can use the application as if they had signed up initially, and enforce some reasonable expections around data ownership and privacy even though they are using a guest account.

## Infrastructure & Background

Literal is built on [AWS Amplify](https://aws.amazon.com/amplify/). Amplify provides a high-level SDK as well as boilerplate integration and deployment code for a bundle of AWS services, namely [DynamoDB](https://aws.amazon.com/dynamodb/), [S3](https://aws.amazon.com/s3/), [Cognito](https://aws.amazon.com/cognito/), [Lambda](https://aws.amazon.com/lambda/), and [AppSync](https://aws.amazon.com/appsync/). At its core, Amplify is a CLI that generates CloudFormation templates for these services, meaning that it's easy to extend and further configure the resulting infrastructure deployment. 

Amplify has some out of the box support for guest authentication. For our purposes here, we'll mostly be focusing on just two of these services:

- Cognito: Provides authentication, user registry, and access control. Literal uses Cognito as its identity provider.
- AppSync: Fully managed GraphQL service. Literal uses AppSync to expose its GraphQL API.

Cognito has [native support for guest authentication](https://docs.aws.amazon.com/location/latest/developerguide/authenticating-using-cognito.html), in which it mints an unauthenticated user identity as part of an [identity pool](https://docs.aws.amazon.com/cognito/latest/developerguide/identity-pools.html). This unauthenticated guest user has an identity ID and can be given permissions to AWS resources through an unauthenticated IAM role. Of course, Cognito also supports authenticated user identities via a [user pool](https://docs.aws.amazon.com/cognito/latest/developerguide/cognito-user-identity-pools.html) associated with the identity pool, which assigns a corresponding authenticted IAM role to the user. The IAM role provides for a basic level of access control, as we can limit particular service and functionality to different authentication contexts. Taken loosely: User Pools are responsible for authentication, Identity Pools are responsible for authorization.

In AppSync, GraphQL field resolvers are defined as [Apache Velocity](https://velocity.apache.org/) (VTL) templates, and Amplify supports generating resolvers automatically from a GraphQL schema definition language (SDL) file. At a high level, GraphQL resolvers share a similar responsibility to a traditional REST endpoint, i.e. the resolver performs some authorization logic, fetches data from some underlying data source, and returns a response that fulfills the GraphQL field interface. Amplify's resolver scaffolding includes support for deriving authorization logic from decorators applied in the GraphQL SDL. Authorization checks are applied against the Cognito-based authorization credentials included with the request, which AppSync helpfully decodes and validates for us ahead of handing the execution context over to the resolver itself.

As documented in the [AppSync Resolver Context Reference](https://docs.aws.amazon.com/appsync/latest/devguide/resolver-context-reference.html#aws-appsync-resolver-context-reference-identity), the identity context included with a request against AppSync from an unauthenticated Cognito identity (i.e. IAM authentication) has the following shape:

<script src="https://gist.github.com/javamonn/19fbd1ed860c40d0eb269eb5396e6f6e.js"></script>

While the identity context of a request from an authenticated Cognito identity (i.e. Cognito User Pool authentication) looks like the following:

<script src="https://gist.github.com/javamonn/13b527a45ee765c7650e5d02985abbb7.js"></script>

## Implementing an Authenticated AppSync Resolver

Putting this together, we can take a look at how Amplify generates an AppSync query resolver by defining a simple model in SDL with "owner" authorization. This authorization pattern enforces that only the authenticated user that created an instance of this model should be able to query for it, i.e. a simplified case of data that is private for a particular user.

For the given SDL:

<script src="https://gist.github.com/javamonn/467a54a7f0612acda54c1ffa256aa437.js"></script>

We define a `SimpleAnnotation` model backed by a DynamoDB table, which uses `identityId` as the DynamoDB partition key and the field for performing "owner" authorization checks against, and uses an `id` field used as the DynamoDB sort key. If we consider the associated generated "get item" query for this model, Amplify generates the following resolver request, which defines how to map the GraphQL field to an operation against DynamoDB:

<script src="https://gist.github.com/javamonn/96f2d1aae9d83a4888aab03cd5d4666b.js"></script>

This is a simple passthrough, using the GraphQL query input variables containing the model keys (i.e. `identityId` and `id`) to execute a DynamoDB `GetItem` operation. The resolver response, which defines how to map the result of the DynamoDB operation to the response used for the GraphQL field resolution, contains the more relevant authorization logic:

<script src="https://gist.github.com/javamonn/9e7c2bcde6b55487675c3de65134d15f.js"></script>

Here we can see checks against the authenticated identity context we discussed above. Although contingent on your ability to read VTL, you should hopefully be able to see that [this logic](https://gist.github.com/javamonn/9e7c2bcde6b55487675c3de65134d15f#file-query-getsimplifiedannotation-res-vtl-L20-L34) is checking if the identity id of the user (contained in `$ctx.identity.claims.username`, i.e. the username of the Cognito Identity Pools user) making the request matches the identity id of the model (`SimpleAnnotation.identityId`) we retrieved from DynamoDB. And thus we acheive user authorization, at least in an authenticated context.

Unfortunately, Amplify does not yet support automatic resolver generation implementing "owner" authorization in an _unauthenticated context_. Other authorization patterns for [guest users are supported](https://docs.amplify.aws/cli/graphql-transformer/auth#allowed-authorization-mode-vs-provider-combinations), but nothing approximates our desired "data should be private to the creating user" pattern. 

To summarize the situation so far:
- Using Cognito, we can authenticate users using Cognito User Pools.
- Using Cognito, we can also authenticate "Guest users", using an unauthenticated identity associated with an Identity Pool.
- Using AppSync, we can authorize requests from authenticated users using the automatically scaffolded resolvers.
- We do not currently have a way to authorize requests from unauthenticated guest users.

## Adding owner authorization support for guest users

As mentioned above, Amplify is not a black-box framework, and can be thought of as a CLI that generates artifacts for deployment - in this case, mostly VTL templates and CloudFormation YAML files. Because of this, we can relatively easily extend the default resolver generation in order to add custom logic to implement "owner" authorization for unauthenticated users.

Keeping the AppSync interface that we observed above for unauthenticated the Cognito identity context in mind, the VTL required for supporting owner authorization for unauthenticated users ("IAM Authorization") and authenticated users ("User Pool Authorization") is relatively simple:

<script src="https://gist.github.com/javamonn/9ade7d2d4c87ef95112f1d9dfaafa77e.js"></script>

This snippet would replace the authorization logic we saw in the generated resolver response that we saw above. If the request is unauthenticated, we'll use the Cognito identity ID associated with the unauthenticated identity for checking record ownership, otherwise we'll use the username associated with the Cognito User Pool record. Literal uses a set of Node.js scripts to post-process VTL templates according to a simple DSL in order to automate this replacement across resolvers. 

This new resolver works as expected, with one hitch: though identity ids are stable as long as a user remains within an authentication context (i.e. they'll retain the same identity id as long as they remain unauthenticated, or remain authenticated), the identity id _does change_ when a user transitions contexts, such as when they create an account after trying Literal out. As far as I can tell, there's no stable identifier that's shared between contexts that's readily usable from AppSync. The [Cognito documentation suggests that the underlying "Cognito Identity ID" would remain stable](https://docs.aws.amazon.com/cognito/latest/developerguide/switching-identities.html), but we lose access to that field within the authenticated identity AppSync context. This problem manifests in that while the data owner remains the same (i.e. it's the same physical user using the application), all data created under the previous identity is written with the old identity ID, so subsequent ownership checks would fail.

Literal addresses this by performing a data migration to effectively merge identities when a user signs up or signs in for the first time. Upon this initial authentication event, a GraphQL Mutation is fired by the application. If the requester can provide valid credentials for both the old and new identities, all data associated with the old identity (DynamoDB records, S3 objects, etc) are rewritten to use the new identity ID. This is less than ideal, though in practice involves a limited amount of data being touched as guest users are by definition new users. Alternatively, an improvement on this front to avoid a migration could be to encode a custom claim on the Cognito User Pool JWT that references the identity ID of the previously used unauthenticated context and check that value for authorization purposes.

We now have covered how guest authentication and authorization is implemented within Literal from the backend and infrastructure perspective, which encompasses the majority of the complexity involved. For completion, we'll walk through the frontend mobile application logic at a high-level.

## Client background & user state model

The Literal Android application is employs a hybrid architecture: the majority of the application interface is rendered by React executed within a WebView, and logic that is best delegated to the platform is executed within the native application directly. Delegated logic includes platform-specific APIs not available within the web context (e.g. an integration with the Android share sheet and intent system), but also logic in which delegating to native offers better guarantees or functionality. This includes authentication. The Amplify Android SDK drives most of the logic we'll see below, though the Amplify JS SDK is ultimately used in order to issue requests against AppSync directly from JavaScript.

Literal models the user's authentication state as follows:

<figure>
  <script src="https://gist.github.com/javamonn/399583cb2ab7fdf8fb9c59e4374fd6b2.js"></script>
  <figcaption>
    <a href="https://github.com/literal-io/literal/blob/21e3ead240b7c9c28e936ec3174fc464cf3e87de/packages/web/src/Providers/Providers_Authentication/Providers_Authentication_User.re#L1-L26">source</a>
  </figcaption>
</figure>

This is a superset of the identity contexts we looked at on the backend; `GuestUser` corresponds to unauthenticated Cognito identity, while `SignedInUser` corresponds to the authenticated User Pool record. Each contains the identity ID we use for designating ownership and the appropriate credentials. The unauthenticated credentials take the form of an AWS access key ID and secret key (which you're familiar with if you've ever set up the AWS CLI locally, or integrated the SDK), while the authenticated credentials take the form of a JWT. Amplify manages these credentials for the most part. Additionally, we have a couple of other user states to capture the complexities of a client application:

- `Unknown`: The state a user starts out in on application initialization, and retains while we check for the existence of valid credentials.
- `SignedInUserMergingIdentities`: Models the "in-between contexts" state, where a user previously in the `GuestUser` state is in the process of transitioning to the `SignedInUser` state and we are migrating their identities on the backend.
- `SignedOutPromptAuthentication`: Models when a previously authenticated user transitions back to an unauthenticated state, either because their session became invalid, or they purposefully logged out. In either case we don't want to create a new guest user, and instead actually do want to prompt the user for re-authentication.

A [React context](https://reactjs.org/docs/context.html) is used to distribute the user state throughout the React application as well as to manage the delegation of the authentication logic to the Android application.

On application boot, an event is dispatched to the native context in order to determine the initial user state:

<figure>
  <script src="https://gist.github.com/javamonn/0b38609066bbc4d791f0d74f21ab99d1.js"></script>
  <figcaption>
    <a href="https://github.com/literal-io/literal/blob/21e3ead240b7c9c28e936ec3174fc464cf3e87de/packages/web/src/Providers/Providers_Authentication/Providers_Authentication.re#L32">source</a>
</figure>

## Implementing guest authentication

This event is handled within Android by a listener attached to the WebView:

<figure>
  <script src="https://gist.github.com/javamonn/bdae723e3b898abe027eb02b806c72b7.js"></script>
  <figcaption>
    <a href="https://github.com/literal-io/literal/blob/21e3ead240b7c9c28e936ec3174fc464cf3e87de/packages/android/app/src/main/java/io/literal/ui/fragment/AppWebView.java#L234-L236">source</a>
  </figcaption>
</figure>

On its initial execution, `handleGetUser` will use the Amplify SDK to derive the persisted user state according to the state definitions we detailed above. For our purposes here we can focus in on `GuestUser` case, where a user has not previously signed in. A call to [`AWSMobileClient.getCredentials`](https://aws-amplify.github.io/aws-sdk-android/docs/reference/com/amazonaws/mobile/client/AWSMobileClient.html#getCredentials--) retrieves an instance of [`AWSSessionCredentials`](https://docs.aws.amazon.com/AWSJavaSDK/latest/javadoc/com/amazonaws/auth/AWSSessionCredentials.html), which is the access key ID and secret key unauthenticated credential pair discussed above. the first call through to this method effectively results in the "registration" of the unauthenticated identity within the Cognito Identity Pool. The stable Cognito identity ID of the unauthenticated user is then retrieved through the Amplify SDK via `AWSMobileClient.getIdentityId`. 

## Putting it all together

Once the `GuestUser` model is instantiated, it is dispatched back to the JavaScript context, where it is then utilized by Amplify. To bring this all together, we can look at the CURL equivalent of the the `getSimpleAnnotation` GraphQL Query, authenticated with guest credentials, that we earlier discussed from a backend perspective:

<script src="https://gist.github.com/javamonn/4116a461ad2e76676bc0136848efd423.js"></script>

Notably, Amplify authenticates the request via the `authorization` and `x-amz-security-token` headers, which include the access key ID and secret key, and which AppSync will validate and decode into the unauthenticated identity context used for performing authorization checks.

Beyond this, the mobile application contains functionality to implement user registration, sign in, and triggering the user state migration, but I'll leave out detailing these aspects in depth.

Hopefully, the details encompassed here assist in determining the relative complexity of implementing an authentication system with a state for guest users, as well as illustrate how one would assemble such a system using AWS Amplify, Cognito, and AppSync.

<details open>
  <summary>Notes</summary>
  <p> If you have thoughts, feedback, or comments about this post, please contribute to the discussion <a href="https://github.com/literal-io/literal/discussions/102">here</a>.</p>
</details>
