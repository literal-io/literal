type t;

[@bs.val] [@bs.scope ("globalThis")] [@bs.return nullable]
external inst: option(t) = "literalWebview";
[@bs.send] external isWebview: t => bool = "isWebview";
