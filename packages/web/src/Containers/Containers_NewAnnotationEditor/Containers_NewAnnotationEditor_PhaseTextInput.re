open Containers_NewAnnotationEditor_GraphQL;

[@react.component]
let make = (~currentUser) => {
  let (textValue, setTextValue) = React.useState(() => "");
  let (tagsValue, setTagsValue) =
    React.useState(() =>
      [|
        Containers_AnnotationEditor_Types.{
          text: Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionLabel,
          id:
            Some(
              Lib_GraphQL.AnnotationCollection.(
                makeIdFromComponent(
                  ~creatorUsername=
                    currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                  ~annotationCollectionIdComponent=recentAnnotationCollectionIdComponent,
                  (),
                )
              ),
            ),
          href: None,
        },
      |]
    );
  let (pendingTagValue, setPendingTagValue) = React.useState(_ => "");

  let (createAnnotationMutation, _s, _f) =
    ApolloHooks.useMutation(CreateAnnotationMutation.definition);

  let handleSave = () => {
    let idPromise =
      Lib_GraphQL.Annotation.makeId(
        ~creatorUsername=
          AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
        ~textualTargetValue=textValue,
      );

    let bodyPromise =
      tagsValue
      ->Belt.Array.map(tag => {
          let id =
            switch (tag.id) {
            | Some(id) => Js.Promise.resolve(id)
            | None =>
              Lib_GraphQL.AnnotationCollection.makeId(
                ~creatorUsername=
                  AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
                ~label=tag.text,
              )
            };
          id
          |> Js.Promise.then_(id =>
               Js.Promise.resolve({
                 "textualBody":
                   Some({
                     "id": Some(id),
                     "value": tag.text,
                     "purpose": Some([|`TAGGING|]),
                     "rights": None,
                     "accessibility": None,
                     "format": Some(`TEXT_PLAIN),
                     "textDirection": Some(`LTR),
                     "language": Some(`EN_US),
                     "processingLanguage": Some(`EN_US),
                     "type": Some(`TEXTUAL_BODY),
                   }),
                 "externalBody": None,
                 "choiceBody": None,
                 "specificBody": None,
               })
             );
        })
      ->Js.Promise.all;

    let _ =
      Js.Promise.all2((idPromise, bodyPromise))
      |> Js.Promise.then_(((id, body)) => {
           let input = {
             "context": [|Lib_GraphQL.Annotation.defaultContext|],
             "type": [|`ANNOTATION|],
             "id": id,
             "created": None,
             "modified": None,
             "generated": None,
             "audience": None,
             "canonical": None,
             "stylesheet": None,
             "via": None,
             "motivation": Some([|`HIGHLIGHTING|]),
             "creatorUsername":
               AwsAmplify.Auth.CurrentUserInfo.(currentUser->username),
             "annotationGeneratorId": None,
             "target": [|
               {
                 "textualTarget":
                   Some({
                     "format": Some(`TEXT_PLAIN),
                     "language": Some(`EN_US),
                     "processingLanguage": Some(`EN_US),
                     "textDirection": Some(`LTR),
                     "accessibility": None,
                     "rights": None,
                     "value": textValue,
                     "id": None,
                   }),
                 "externalTarget": None,
               },
             |],
             "body": Js.Array2.length(body) > 0 ? Some(body) : None,
           };

           let variables = CreateAnnotationMutation.makeVariables(~input, ());

           let _ =
             Containers_NewAnnotationEditor_Apollo.updateCache(
               ~currentUser,
               ~input,
             );
           let _ = createAnnotationMutation(~variables, ());
           let _ =
             Routes.CreatorsIdAnnotationCollectionsId.(
               Next.Router.pushWithAs(
                 staticPath,
                 path(
                   ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
                   ~creatorUsername=currentUser.username,
                 ),
               )
             );
           Js.Promise.resolve();
         });
    ();
  };

  let handleTextChange = value => setTextValue(_ => value);
  let handleTagsChange = value => setTagsValue(_ => value);
  let handlePendingTagChange = value => setPendingTagValue(_ => value);
  let handlePendingTagCommit = value => {
    let _ =
      setTagsValue(tags =>
        Js.Array2.concat(
          [|
            Containers_AnnotationEditor_Types.{
              text: value,
              id: None,
              href: None,
            },
          |],
          tags,
        )
      );
    let _ = setPendingTagValue(_ => "");
    let _ =
      Lib_GraphQL.AnnotationCollection.makeId(
        ~creatorUsername=currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
        ~label=value,
      )
      |> Js.Promise.then_(id => {
           let _ =
             setTagsValue(tags => {
               tags
               ->Belt.Array.getIndexBy(tag => tag.text === value)
               ->Belt.Option.map(idx => {
                   let copy = Js.Array2.copy(tags);
                   let _ =
                     Js.Array2.spliceInPlace(
                       tags,
                       ~pos=idx,
                       ~remove=1,
                       ~add=[|
                         Containers_AnnotationEditor_Types.{
                           text: value,
                           id: Some(id),
                           href: None,
                         },
                       |],
                     );
                   copy;
                 })
               ->Belt.Option.getWithDefault(tags)
             });
           Js.Promise.resolve();
         });
    ();
  };

  <div
    className={Cn.fromList([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={Cn.fromList(["px-6", "pb-4", "py-16"])}>
      <TextInput.Annotation
        onTextChange=handleTextChange
        onTagsChange=handleTagsChange
        textValue
        tagsValue
        placeholder="Lorem Ipsum..."
        autoFocus=true
      />
    </div>
    <div
      className={Cn.fromList([
        "absolute",
        "bottom-0",
        "left-0",
        "right-0",
        "flex",
        "flex-col",
        "items-end",
      ])}>
      {Js.String.length(textValue) > 0
         ? <FloatingActionButton
             onClick={_ev => handleSave()}
             className={Cn.fromList(["m-6", "z-10"])}>
             <Svg
               placeholderViewBox="0 0 24 24"
               className={Cn.fromList([
                 "w-10",
                 "h-10",
                 "pointer-events-none",
               ])}
               icon=Svg.done_
             />
           </FloatingActionButton>
         : React.null}
      <TextInput_Tags
        className={Cn.fromList(["px-2", "bg-black", "z-10"])}
        onValueChange=handlePendingTagChange
        onValueCommit=handlePendingTagCommit
        value=pendingTagValue
      />
    </div>
  </div>;
};
