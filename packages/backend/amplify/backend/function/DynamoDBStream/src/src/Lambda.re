[@decco]
type stringField = {
  [@decco.key "S"]
  string,
};

[@decco]
type highlightTagAttributeMap = {
  highlightId: stringField,
  tagId: stringField,
  owner: stringField,
};

[@decco]
type dynamoStreamItem('a) = {
  [@decco.key "ApproximateCreationTimeDate"]
  approximateCreationTimeDate: string,
  [@decco.default None] [@decco.key "NewImage"]
  newImage: option('a),
  [@decco.default None] [@decco.key "OldImage"]
  oldImage: option('a),
  [@decco.key "SequenceNumber"]
  sequenceNumber: string,
  [@decco.key "SizeBytes"]
  sizeBytes: string,
  [@decco.key "StreamViewType"]
  streamViewType: string,
};

type tableName =
  | HighlightTag;
let tableNameFromEventARN = arn =>
  Js.String2.includes(arn, "HighlightTag") ? Some(HighlightTag) : None;

[@decco]
type record = {
  eventName: string,
  eventSource: string,
  eventId: string,
  eventVersion: string,
  awsRegion: string,
  eventSourceARN: string,
  dynamodb: dynamoStreamItem(highlightTagAttributeMap),
};

[@decco]
type event = {
  [@decco.key "Records"]
  records: array(record),
};

type context;
type cb = (. Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
type handler = (Js.Json.t, context, cb) => Js.Promise.t(option(Js.Json.t));
