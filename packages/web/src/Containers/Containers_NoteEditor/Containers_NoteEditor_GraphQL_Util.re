let makeTagId = (~text, ~owner) => owner ++ "-" ++ text;

let makeHighlightTagId = (~highlightId, ~tagId) =>
  highlightId ++ "-" ++ tagId;
