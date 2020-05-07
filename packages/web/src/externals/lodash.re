type debounced1('a, 'b) = (. 'a) => 'b;
type debounced2('a, 'b, 'c) = (. 'a, 'b) => 'c;
type debounced3('a, 'b, 'c, 'd) = (. 'a, 'b, 'c) => 'd;

[@bs.send] external flush1: debounced1('a, 'b) => unit = "flush";
[@bs.send] external flush2: debounced2('a, 'b, 'c) => unit = "flush";
[@bs.send] external flush3: debounced3('a, 'b, 'c, 'd) => unit = "flush";

[@bs.module]
external debounce1: ((. 'a) => 'b, int) => debounced1('a, 'b) =
  "lodash/debounce";

[@bs.module]
external debounce2: ((. 'a, 'b) => 'c, int) => debounced2('a, 'b, 'c) =
  "lodash/debounce";

[@bs.module]
external debounce3: ((. 'a, 'b, 'c) => 'd, int) => debounced3('a, 'b, 'c, 'd) =
  "lodash/debounce";
