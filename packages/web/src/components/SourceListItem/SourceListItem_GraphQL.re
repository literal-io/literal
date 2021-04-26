module AnnotationCollectionFragment = [%graphql
  {|
      fragment SourceListItem_AnnotationCollection on AnnotationCollection {
        id
        label
        total
      }
  |}
];
