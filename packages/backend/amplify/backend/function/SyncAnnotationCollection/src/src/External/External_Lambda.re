[@decco]
type annotationBody = {
  [@decco.key "type"]
  type_: string,
  purpose: option(array(string)),
  value: string
};

[@decco]
type annotation = {
  id: string,
  body: option(array(annotationBody)),
};

[@decco]
type event = {
  before: option(annotation),
  after: option(annotation),
};
