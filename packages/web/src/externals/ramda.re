[@bs.module]
external mergeDeepLeft: ('a, 'b) => 'c = "ramda/src/mergeDeepLeft";

[@bs.module] external _assoc: (string, 'a, 'b) => 'b = "ramda/src/assoc";
[@bs.module]
external _sortBy: ('a => 'b, Js.Array.t('a)) => Js.Array.t('a) =
  "ramda/src/sortBy";
[@bs.module]
external _sort: ((. 'a, 'a) => int, Js.Array.t('a)) => Js.Array.t('a) =
  "ramda/src/sortBy";
[@bs.module]
external _uniqBy: ('a => 'b, Js.Array.t('a)) => Js.Array.t('a) =
  "ramda/src/uniqBy";

/** pipe-first variants, ramda is pipe-last **/
let sortBy = (arr, pred) => _sortBy(pred, arr);
let sort = (arr, pred) => _sort(pred, arr);
let assoc = (obj, key, data) => _assoc(key, data, obj);
let uniqBy = (arr, pred) => _uniqBy(pred, arr);
