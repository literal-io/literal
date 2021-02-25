[@bs.module]
external mergeDeepLeft: ('a, 'b) => 'c = "ramda/src/mergeDeepLeft";

[@bs.module] external assoc: (string, 'a, 'b) => 'b = "ramda/src/assoc";

[@bs.module]
external sortBy: ('a => 'b, Js.Array.t('a)) => Js.Array.t('a) =
  "ramda/src/sortBy";

[@bs.module]
external sort: ((. 'a, 'a) => int, Js.Array.t('a)) => Js.Array.t('a) =
  "ramda/src/sortBy";
