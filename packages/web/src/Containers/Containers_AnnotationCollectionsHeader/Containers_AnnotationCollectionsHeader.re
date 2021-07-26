let collectionTypes = [|`TAG_COLLECTION, `SOURCE_COLLECTION|];

[@react.component]
let make = (~onActiveCollectionTypeChange, ~activeCollectionType) => {
  <>
    <Header
      className={Cn.fromList([
        "flex",
        "flex-row",
        "px-4",
        "items-center",
        "bg-darkAccent",
        "flex-shrink-0",
      ])}>
      <h1
        className={Cn.fromList([
          "font-sans",
          "font-bold",
          "text-lg",
          "text-lightPrimary",
        ])}>
        {React.string("Collections")}
      </h1>
    </Header>
    <MaterialUi.Tabs
      variant=`FullWidth
      value={MaterialUi_Types.Any(activeCollectionType)}
      onChange={(_ev, newActiveCollectionType) => {
        let unpacked = MaterialUi_Types.anyUnpack(newActiveCollectionType);
        let _ = onActiveCollectionTypeChange(unpacked);
        ();
      }}
      classes={MaterialUi.Tabs.Classes.make(
        ~root=
          Cn.fromList([
            "bg-darkAccent",
            "border-b",
            "border-dotted",
            "border-lightDisabled",
            "flex-shrink-0",
          ]),
        ~indicator=Cn.fromList(["bg-lightSecondary"]),
        (),
      )}>
      {collectionTypes->Belt.Array.map(collectionType => {
         let (tabIcon, tabLabel) =
           switch (collectionType) {
           | `SOURCE_COLLECTION => (Svg.article, "Sources")
           | `TAG_COLLECTION => (Svg.label, "Tags")
           };
         <MaterialUi.Tab
           value={MaterialUi_Types.Any(collectionType)}
           classes={MaterialUi.Tab.Classes.make(
             ~root=Cn.fromList(["text-lightPrimary", "font-sans", "italic"]),
             (),
           )}
           icon={
             <Svg
               className={Cn.fromList(["pointer-events-none"])}
               style={ReactDOMRe.Style.make(
                 ~width="1.75rem",
                 ~height="1.75rem",
                 (),
               )}
               icon=tabIcon
             />
           }
           label={React.string(tabLabel)}
         />;
       })}
    </MaterialUi.Tabs>
  </>;
};
