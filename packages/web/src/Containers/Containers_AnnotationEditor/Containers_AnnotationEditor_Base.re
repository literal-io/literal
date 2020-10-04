open Styles;
open Containers_AnnotationEditor_Base_Types;

let styles = [%raw "require('./Containers_AnnotationEditor_Base.module.css')"];

[@react.component]
let make =
    (
      ~annotationFragment as annotation=?,
      ~onChange,
      ~currentUser,
      ~autoFocus=?,
      ~placeholder=?,
    ) => {
  let (textState, setTextState) =
    React.useState(() =>
      annotation
      ->Belt.Option.flatMap(a =>
          a##target
          ->Belt.Array.getBy(target =>
              switch (target) {
              | `TextualTarget(_) => true
              | `ExternalTarget(_) => false
              }
            )
        )
      ->Belt.Option.flatMap(target =>
          switch (target) {
          | `TextualTarget(target) => Some(target##value)
          | `ExternalTarget(_) => None
          }
        )
      ->Belt.Option.getWithDefault("")
    );

  let _ =
    React.useEffect1(
      () => {
        let _ = onChange(textState);
        None;
      },
      [|textState|],
    );

  let handleTextChange = s => setTextState(_ => s);

  let tagsValue =
    annotation
    ->Belt.Option.flatMap(a => a##body)
    ->Belt.Option.map(bodies =>
        bodies->Belt.Array.keepMap(body =>
          switch (body) {
          | `TextualBody(body) when Lib_GraphQL.Annotation.isBodyTag(body) =>
            let href =
              body##id
              ->Belt.Option.map(id =>
                  Lib_GraphQL.AnnotationCollection.(
                    makeIdFromComponent(
                      ~annotationCollectionIdComponent=idComponent(id),
                      ~creatorUsername=
                        currentUser->AwsAmplify.Auth.CurrentUserInfo.username,
                      ~origin=
                        Webapi.Dom.(window->Window.location->Location.origin),
                      (),
                    )
                  )
                );
            Some(TagsList.{text: body##value, id: body##id, href});
          | `Nonexhaustive => None
          | _ => None
          }
        )
      )
    ->Belt.Option.getWithDefault([|
        {
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
      |]);

  <div
    className={cn([
      "w-full",
      "h-full",
      "bg-black",
      "flex",
      "flex-col",
      "overflow-y-auto",
    ])}>
    <div className={cn(["px-6", "pb-4", "pt-16"])}>
      <TextInput.Annotation
        onTextChange=handleTextChange
        textValue=textState
        tagsValue
        ?placeholder
        ?autoFocus
      />
    </div>
  </div>;
};
