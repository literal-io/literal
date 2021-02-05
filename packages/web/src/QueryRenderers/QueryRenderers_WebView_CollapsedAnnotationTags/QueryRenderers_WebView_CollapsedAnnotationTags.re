module Data = {
  [@react.component]
  let make = (~currentUser, ~annotation) => {
    let tags = [||];
    <div
      className={Cn.fromList([
        "w-full",
        "h-full",
        "bg-black",
        "flex",
        "flex-col",
      ])}>
      <div
        className={Cn.fromList([
          "flex",
          "flex-row",
          "flex-1",
          "border-b",
          "border-lightPrimary",
          "border-dotted",
        ])}>
        <ul
          className={Cn.fromList([
            "flex-1",
            "overflow-x-auto",
            "py-2",
            "border-r",
            "border-lightPrimary",
            "border-dotted",
          ])}>
          {tags
           ->Belt.Array.map(text =>
               <div
                 className={Cn.fromList([
                   "mr-4",
                   "border-b",
                   "border-dotted",
                   "border-lightPrimary",
                 ])}>
                 <span
                   className={Cn.fromList([
                     "font-sans",
                     "text-lightSecondary",
                     "font-medium",
                     "text-base",
                     "normal-case",
                   ])}>
                   {React.string(text)}
                 </span>
               </div>
             )
           ->React.array}
        </ul>
        <div
          className={Cn.fromList([
            "flex",
            "justify-center",
            "items-center",
            "h-12",
            "w-12",
          ])}>
          <Svg
            placeholderViewBox="0 0 24 24"
            className={Cn.fromList(["w-8", "h-8", "pointer-events-none"])}
            icon=Svg.arrowUp
          />
        </div>
      </div>
      <div className={Cn.fromList(["flex", "flex-row", "flex-1"])}>
        <TextInput_Tags
          disabled=true
          value=""
          onValueChange={_ => ()}
          onValueCommit={_ => ()}
        />
      </div>
    </div>;
  };
};

module Loading = {
  [@react.component]
  let make = () => {
    <div />;
  };
};

[@react.component]
let make =
    (
      ~rehydrated,
      ~authentication: Hooks_CurrentUserInfo_Types.state,
      ~annotation,
    ) => {
  switch (rehydrated, authentication) {
  | (_, Unauthenticated)
  | (_, Loading)
  | (false, _) => <Loading />
  | (true, Authenticated(currentUser)) => <Data currentUser annotation />
  };
};
