type t;

[@bs.new] external make: string => t = "URLSearchParams";
[@bs.new] external makeWithJson: Js.Json.t => t = "URLSearchParams";
[@bs.send] external toString: t => string = "toString";
