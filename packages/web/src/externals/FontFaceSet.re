type t;

include Webapi.Dom.EventTarget.Impl({
  type nonrec t = t;
});

[@bs.scope "document"] [@bs.val] external inst: t = "fonts";

[@bs.deriving jsConverter]
type status = [ | [@bs.as "loading"] `Loading | [@bs.as "loaded"] `Loaded];
[@bs.get] external _status: t => string = "status";
let status = inst => inst->_status->statusFromJs->Belt.Option.getExn;

[@bs.get] external ready: t => Js.Promise.t(unit) = "ready";

module LoadingDoneEvent = {
  [@bs.deriving accessors]
  type fontface = {
    family: string,
    status: string,
    style: string,
  };

  [@bs.deriving accessors]
  type t = {fontfaces: array(fontface)};

  include Webapi.Dom.Event.Impl({
    type nonrec t = t;
  });

  external unsafeOfEvent: Dom.event => t = "%identity";
};

[@bs.send] external check: (t, string) => bool = "check";
