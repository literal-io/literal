[@react.component]
let make =
    (
      ~annotationFragment as annotation,
      ~onViewTargetForAnnotation,
      ~isAnnotationVisible,
    ) => {
  let targetWithExternalTarget =
    annotation##target
    ->Belt.Array.keepMap(target =>
        switch (target) {
        | `SpecificTarget(specficTarget) =>
          switch (specficTarget##source) {
          | `ExternalTarget(externalTarget) =>
            Some((specficTarget##specificTargetId, externalTarget))
          | _ => None
          }
        | _ => None
        }
      )
    ->Belt.Array.get(0);

  let _ =
    React.useEffect2(
      () => {
        let _ =
          switch (targetWithExternalTarget) {
          | Some((targetId, _)) when isAnnotationVisible =>
            onViewTargetForAnnotation(
              ~targetId,
              ~annotation,
              ~displayBottomSheet=false,
              (),
            )
          | _ => ()
          };
        None;
      },
      (targetWithExternalTarget, isAnnotationVisible),
    );

  targetWithExternalTarget
  ->Belt.Option.map(((targetId, externalTarget)) => {
      let host =
        externalTarget##externalTargetId->Webapi.Url.make->Webapi.Url.host;

      let handleClick = _ =>
        onViewTargetForAnnotation(
          ~targetId,
          ~annotation,
          ~displayBottomSheet=true,
          (),
        );

      <MaterialUi.Button
        onClick=handleClick
        _TouchRippleProps={
          "classes": {
            "child": Cn.fromList(["bg-white"]),
            "rippleVisible": Cn.fromList(["opacity-75"]),
          },
        }
        fullWidth=false
        size=`Large
        classes={MaterialUi.Button.Classes.make(
          ~root=Cn.fromList(["py-4"]),
          ~label=
            Cn.fromList([
              "text-white",
              "font-sans",
              "font-bold",
              "text-lg",
              "leading-none",
              "italic",
              "px-6",
            ]),
          ~outlined=Cn.fromList(["border-white"]),
          (),
        )}
        variant=`Outlined
        startIcon={
          <div className={Cn.fromList(["w-4", "h-4", "bg-white"])} />
        }>
        {React.string(host)}
      </MaterialUi.Button>;
    })
  ->Belt.Option.getWithDefault(React.null);
};
