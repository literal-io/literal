[%raw "require('isomorphic-fetch')"];

let _ = AwsAmplify.(inst->configure(Constants.awsAmplifyConfig));

module Lambda = {

  [@decco]
  type stringField = {
    [@decco.key "S"]
    string: string
  };

  [@decco]
  type highlightTagAttributeMap = {
    highlightId: stringField,
    tagId: stringField,
    owner: stringField
  };

  [@decco]
  type dynamoStreamItem('a) = {
    [@decco.key "ApproximateCreationTimeDate"]
    approximateCreationTimeDate: string,
    [@decco.key "NewImage"]
    newImage: option('a),
    [@decco.key "OldImage"]
    oldImage: option('a),
    [@decco.key "SequenceNumber"]
    sequenceNumber: string,
    [@decco.key "SizeBytes"]
    sizeBytes: string,
    [@decco.key "StreamViewType"]
    streamViewType: string
  };

  [@decco]
  type record = {
    eventName: string,
    eventSource: string,
    eventId: string,
    eventVersion: string,
    awsRegion: string,
    eventSourceARN: string,
    dynamodb: dynamoStreamItem(highlightTagAttributeMap)
  };

  [@decco]
  type event = {
    [@decco.key "Records"]
    records: array(record)
  };

  type context;
  type cb = (. Js.Nullable.t(Js.Promise.error), Js.Json.t) => unit;
  type handler = (event, context, cb) => Js.Promise.t(option(Js.Json.t));
}

let handler: Lambda.handler = (event, ctx, cb) => {
  Js.log2("event", Js.Json.stringifyAny(event));
  Js.Promise.resolve(None); 
};
