type textEncoder;

[@bs.new] external makeTextEncoder: unit => textEncoder = "TextEncoder";
[@bs.send]
external encode: (textEncoder, string) => Js.TypedArray2.Uint8Array.t =
  "encode";

[@bs.val] [@bs.scope ("window", "crypto", "subtle")]
external digest: (string, Js.TypedArray2.Uint8Array.t) => Js.Promise.t(string) =
  "digest";
