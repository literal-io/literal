
type t;

[@bs.deriving abstract]
type navigator = {userAgent: string};

external asNavigator: Webapi.Dom.Window.navigator => navigator = "%identity";

[@bs.deriving abstract]
type browser = {
  name: string,
  version: string,
};

[@bs.deriving jsConverter]
type browserName = [ | `Firefox | `Chrome | `Safari];

let getBrowserName = browser => browser |> nameGet |> browserNameFromJs;

[@bs.val] [@bs.module "bowser"] external make: string => t = "getParser";

[@bs.send] external getBrowser: t => browser = "getBrowser";

// https://github.com/lancedikson/bowser/blob/master/src/constants.js#L93
[@bs.deriving jsConverter]
type osName = [ 
  | [@bs.as "macOS"] `MacOS
  | `Android
  | [@bs.as "iOS"] `iOS
];

type os = {
  name: string,
  version: string
};

[@bs.send] external getOS: t => os = "getOS";

let getOSName = os => osNameFromJs(os.name);
