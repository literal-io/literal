type instance;

/**
 * amplitude-js has a dependency on window, shim when running in node
 * during local dev and static html export.
 */
let getInstance: unit => instance = [%raw {|
  function () {
    if (window) {
      return require("amplitude-js")
    } else {
      return {
        init: () => {},
        logWithEvent: () => {}
      }
    }
  }
|}];

[@bs.send] external init: (instance, string) => unit = "init";

/* https://help.amplitude.com/hc/en-us/articles/115001361248#settings-configuration-options */
[@bs.deriving abstract]
type initOptions = {
  [@bs.optional]
  batchEvents: bool,
  [@bs.optional]
  cookieExpiration: int,
  [@bs.optional]
  cookieName: string,
  [@bs.optional]
  deviceId: string,
  [@bs.optional]
  deviceIdFromurlParam: bool,
  [@bs.optional]
  domain: string,
  [@bs.optional]
  eventUploadPeriodMillis: int,
  [@bs.optional]
  eventUploadThreshold: int,
  [@bs.optional]
  forceHttps: bool,
  [@bs.optional]
  includeGclid: bool,
  [@bs.optional]
  includeReferrer: bool,
  [@bs.optional]
  includeUtm: bool,
  [@bs.optional]
  language: string,
  [@bs.optional]
  logLevel: string,
  [@bs.optional]
  optOut: bool,
  [@bs.optional]
  platform: string,
  [@bs.optional]
  saveEvents: bool,
  [@bs.optional]
  savedMaxCount: int,
  [@bs.optional]
  saveParamsReferrerOncePerSession: bool,
  [@bs.optional]
  sessionTimeout: int,
  [@bs.optional]
  trackingOptions: Js.Dict.t(bool),
  [@bs.optional]
  unsertParamsReferrerOnNewSession: bool,
  [@bs.optional]
  uploadBatchSize: int,
};

[@bs.send]
external initWithOptions:
  (instance, option(string), option(initOptions), option(unit => unit)) =>
  unit =
  "init";

[@bs.send] external logEvent: (instance, string) => unit = "logEvent";

[@bs.send]
external logEventWithProperties: (instance, string, Js.Json.t) => unit =
  "logEvent";
