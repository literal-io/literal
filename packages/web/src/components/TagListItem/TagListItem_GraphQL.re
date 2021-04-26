module AnnotationCollectionFragment = [%graphql
  {|
      fragment TagListItem_AnnotationCollection on AnnotationCollection {
        id
        label
        total
      }
  |}
];
