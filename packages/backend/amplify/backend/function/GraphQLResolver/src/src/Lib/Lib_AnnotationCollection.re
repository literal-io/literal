let recentAnnotationCollectionIdComponent = "034a7e52c5c9534b709dc1dba403868399b0949f7c1933a67325c22077ffc221";
let recentAnnotationCollectionLabel = "recent";

let makeIdFromComponent =
    (
      ~creatorUsername,
      ~annotationCollectionIdComponent,
      ~origin=Lib_Constants.apiOrigin,
      (),
    ) =>
  origin
  ++ "/creators/"
  ++ creatorUsername
  ++ "/annotation-collections/"
  ++ annotationCollectionIdComponent;
