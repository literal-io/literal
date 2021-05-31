open QueryRenderers_AnnotationCollectionsDrawer_GraphQL;
let styles = [%raw
  "require('./QueryRenderers_AnnotationCollectionsDrawer.module.css')"
];

[@bs.deriving jsConverter]
type collectionType = [ | `TAG_COLLECTION | `SOURCE_COLLECTION];

let collectionTypes = [|`TAG_COLLECTION, `SOURCE_COLLECTION|];

module Error = {
  [@react.component]
  let make = (~error=?) => {
    let _ =
      React.useEffect1(
        () => {
          Js.log(error);
          None;
        },
        [|error|],
      );

    React.null;
  };
};

module Main = {
  [@react.component]
  let make =
      (
        ~activeCollectionType,
        ~onActiveCollectionTypeChange,
        ~renderTagsCollection,
        ~renderSourcesCollection,
      ) => {
    let scrollSnapListRef = React.useRef(Js.Nullable.null);
    let didChangeIdxViaScroll = React.useRef(false);

    let activeIdx =
      collectionTypes
      ->Belt.Array.getIndexBy(collectionType =>
          switch (collectionType, activeCollectionType) {
          | (`TAG_COLLECTION, `TAG_COLLECTION)
          | (`SOURCE_COLLECTION, `SOURCE_COLLECTION) => true
          | _ => false
          }
        )
      ->Belt.Option.getWithDefault(0);

    let _ =
      React.useEffect1(
        () => {
          if (!didChangeIdxViaScroll.current) {
            let _ =
              scrollSnapListRef.current
              ->Js.Nullable.toOption
              ->Belt.Option.forEach(scrollSnapList => {
                  scrollSnapList##scrollToIdx(~behavior="smooth", activeIdx)
                });
            ();
          };
          didChangeIdxViaScroll.current = false;
          None;
        },
        [|activeIdx|],
      );

    let handleIdxChange = newIdx => {
      if (newIdx != activeIdx) {
        didChangeIdxViaScroll.current = true;
        let _ =
          collectionTypes
          ->Belt.Array.get(newIdx)
          ->Belt.Option.forEach(onActiveCollectionTypeChange);
        ();
      };
    };

    <ScrollSnapList.Container
      ref=scrollSnapListRef
      direction=ScrollSnapList.Horizontal
      onIdxChange=handleIdxChange
      initialIdx=activeIdx>
      {collectionTypes->Belt.Array.map(collectionType => {
         <ScrollSnapList.Item
           key={collectionType->collectionTypeToJs}
           direction=ScrollSnapList.Horizontal>
           {switch (collectionType) {
            | `TAG_COLLECTION => renderTagsCollection()
            | `SOURCE_COLLECTION => renderSourcesCollection()
            }}
         </ScrollSnapList.Item>
       })}
    </ScrollSnapList.Container>;
  };
};

module Loading = {
  [@react.component]
  let make = (~activeCollectionType, ~onActiveCollectionTypeChange) => {
    <Main
      activeCollectionType
      onActiveCollectionTypeChange
      renderSourcesCollection={() =>
        <SourceList
          data={Belt.Array.make(20, ())}
          className={Cn.fromList(["p-4"])}
          itemKey={(~item, ~idx) => string_of_int(idx)}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4"])}
          renderItem={(~item, ~idx) => <SourceListItem.Loading />}
        />
      }
      renderTagsCollection={() =>
        <TagList
          data={Belt.Array.make(30, ())}
          itemKey={(~item, ~idx) => string_of_int(idx)}
          className={Cn.fromList(["p-4", "flex", "flex-wrap"])}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4", "mr-4"])}
          renderItem={(~item, ~idx) => <TagListItem.Loading />}
        />
      }
    />;
  };
};

module Data = {
  [@react.component]
  let make =
      (
        ~annotationCollections,
        ~activeCollectionType,
        ~onActiveCollectionTypeChange,
        ~onClose,
      ) => {
    let scrollSnapListRef = React.useRef(Js.Nullable.null);
    let didChangeIdxViaScroll = React.useRef(false);

    let activeIdx =
      collectionTypes
      ->Belt.Array.getIndexBy(collectionType =>
          collectionType === activeCollectionType
        )
      ->Belt.Option.getWithDefault(0);

    let _ =
      React.useEffect1(
        () => {
          if (!didChangeIdxViaScroll.current) {
            let _ =
              scrollSnapListRef.current
              ->Js.Nullable.toOption
              ->Belt.Option.forEach(scrollSnapList => {
                  scrollSnapList##scrollToIdx(~behavior="smooth", activeIdx)
                });
            ();
          };
          didChangeIdxViaScroll.current = false;
          None;
        },
        [|activeIdx|],
      );

    let handleIdxChange = newIdx =>
      if (newIdx != activeIdx) {
        didChangeIdxViaScroll.current = true;
        let _ =
          collectionTypes
          ->Belt.Array.get(newIdx)
          ->Belt.Option.forEach(onActiveCollectionTypeChange);
        ();
      };

    let getItems = collectionType =>
      annotationCollections
      ->Belt.Array.keepMap(i => i)
      ->Belt.Array.keep(item =>
          item##type_
          ->Belt.Array.some(t =>
              switch (t, collectionType) {
              | (`SOURCE_COLLECTION, `SOURCE_COLLECTION)
              | (`TAG_COLLECTION, `TAG_COLLECTION) => true
              | _ => false
              }
            )
        );

    <Main
      activeCollectionType
      onActiveCollectionTypeChange
      renderSourcesCollection={() => {
        <SourceList
          data={getItems(`SOURCE_COLLECTION)}
          className={Cn.fromList(["p-4"])}
          itemKey={(~item, ~idx) => item##id}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4"])}
          renderItem={(~item, ~idx) =>
            <SourceListItem
              annotationCollectionFragment={item##sourceListItem}
              onClick={_ => onClose()}
            />
          }
        />
      }}
      renderTagsCollection={() => {
        <TagList
          data={getItems(`TAG_COLLECTION)}
          className={Cn.fromList(["p-4", "flex", "flex-wrap"])}
          itemKey={(~item, ~idx) => item##id}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4", "mr-4"])}
          renderItem={(~item, ~idx) =>
            <TagListItem
              annotationCollectionFragment={item##tagListItem}
              onClick={_ => onClose()}
            />
          }
        />
      }}
    />;
  };
};

[@react.component]
let make = (~isVisible, ~onClose, ~rehydrated, ~user) => {
  let (activeCollectionType, setActiveCollectionType) =
    React.useState(() => `TAG_COLLECTION);

  let identityId =
    switch (user) {
    | Providers_Authentication_User.GuestUser({identityId})
    | SignedInUser({identityId}) => Some(identityId)
    | _ => None
    };

  let (_, query) =
    ApolloHooks.useQuery(
      ~variables=
        identityId
        ->Belt.Option.map(identityId =>
            ListAnnotationCollections.Query.makeVariables(
              ~creatorUsername=identityId,
              (),
            )
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ListAnnotationCollections.Query.definition,
    );

  let handleActiveCollectionTypeChange = collectionType => {
    let _ = setActiveCollectionType(_ => collectionType);
    ();
  };

  let main =
    switch (query, rehydrated, identityId) {
    | (_, false, _)
    | (_, _, None)
    | ({data: None, loading: true}, _, _) =>
      <Loading
        activeCollectionType
        onActiveCollectionTypeChange=handleActiveCollectionTypeChange
      />
    | ({data: Some(data), loading}, true, Some(_)) =>
      let annotationCollections =
        data##listAnnotationCollections
        ->Belt.Option.flatMap(c => c##items)
        ->Belt.Option.getWithDefault([||]);
      <Data
        annotationCollections
        activeCollectionType
        onActiveCollectionTypeChange=handleActiveCollectionTypeChange
        onClose
      />;
    | ({error: Some(error)}, _, _) => <Error error />
    | ({error: None, data: None, loading: false}, _, _)
    | _ => <Error />
    };

  <MaterialUi.Drawer
    anchor=`Top
    _open=isVisible
    onClose={_ => onClose()}
    classes={MaterialUi.Drawer.Classes.make(
      ~paper=
        Cn.fromList(["bg-lightAccent", "rounded-b-lg", styles##drawerPaper]),
      (),
    )}>
    <Containers_AnnotationCollectionsHeader
      onActiveCollectionTypeChange=handleActiveCollectionTypeChange
      activeCollectionType
      onCloseClicked={() => onClose()}
    />
    main
  </MaterialUi.Drawer>;
};
