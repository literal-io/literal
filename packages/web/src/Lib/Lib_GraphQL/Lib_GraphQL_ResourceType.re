[@bs.deriving jsConverter]
type t = [ | `DATASET | `IMAGE | `SOUND | `TEXT | `VIDEO];

let toJs = tToJs;
