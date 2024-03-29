open QueryRenderers_AnnotationCollections_GraphQL;

[@bs.deriving jsConverter]
type collectionType = [ | `TAG_COLLECTION | `SOURCE_COLLECTION];

let collectionTypes = [|`TAG_COLLECTION, `SOURCE_COLLECTION|];
let defaultCollectionType = `TAG_COLLECTION;
 
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

    let handleIdxChange = newIdx =>
      if (newIdx != activeIdx) {
        didChangeIdxViaScroll.current = true;
        let _ =
          collectionTypes
          ->Belt.Array.get(newIdx)
          ->Belt.Option.forEach(onActiveCollectionTypeChange);
        ();
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
          className={Cn.fromList(["p-4", "pb-16"])}
          itemKey={(~item, ~idx) => string_of_int(idx)}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4"])}
          renderItem={(~item, ~idx) => <SourceListItem.Loading />}
        />
      }
      renderTagsCollection={() =>
        <TagList
          data={Belt.Array.make(30, ())}
          itemKey={(~item, ~idx) => string_of_int(idx)}
          className={Cn.fromList(["p-4", "flex", "flex-wrap", "pb-16"])}
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
          className={Cn.fromList(["p-4", "pb-16"])}
          itemKey={(~item, ~idx) => item##id}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4"])}
          renderItem={(~item, ~idx) =>
            <SourceListItem
              annotationCollectionFragment={item##sourceListItem}
            />
          }
        />
      }}
      renderTagsCollection={() => {
        <TagList
          data={getItems(`TAG_COLLECTION)}
          className={Cn.fromList(["p-4", "flex", "flex-wrap", "pb-16"])}
          itemKey={(~item, ~idx) => item##id}
          itemClassName={(~item, ~idx) => Cn.fromList(["mb-4", "mr-4"])}
          renderItem={(~item, ~idx) =>
            <TagListItem annotationCollectionFragment={item##tagListItem} />
          }
        />
      }}
    />;
  };
};

[@react.component]
let make = (~rehydrated, ~user) => {
  let (activeCollectionType, setActiveCollectionType) =
    React.useState(() => defaultCollectionType);

  let identityId =
    switch (user) {
    | Providers_Authentication_User.GuestUser({identityId})
    | SignedInUser({identityId}) => Some(identityId)
    | _ => None
    };

  let (_, query) =
    ApolloHooks.useQuery(
      ~skip=
        switch (user) {
        | GuestUser(_) when rehydrated => false
        | SignedInUser(_) when rehydrated => false
        | Unknown
        | SignedOutPromptAuthentication
        | _ => true
        },
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
      />;
    | ({error: Some(error)}, _, _) => <Error error />
    | ({error: None, data: None, loading: false}, _, _)
    | _ => <Error />
    };

  <>
    <Containers_AnnotationCollectionsHeader
      onActiveCollectionTypeChange=handleActiveCollectionTypeChange
      activeCollectionType
    />
    main
  </>;
};
