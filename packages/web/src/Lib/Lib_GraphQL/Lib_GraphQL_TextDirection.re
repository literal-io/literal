type t = [ | `LTR | `RTL | `AUTO];

let toString = t =>
  switch (t) {
  | `LTR => "LTR"
  | `RTL => "RTL"
  | `AUTO => "AUTO"
  };
