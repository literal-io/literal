type t = [ | `TEXT_PLAIN | `TEXT_HTML];

let toString = t =>
  switch (t) {
  | `TEXT_PLAIN => "TEXT_PLAIN"
  | `TEXT_HTML => "TEXT_HTML"
  };
