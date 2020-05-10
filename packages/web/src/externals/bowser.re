
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
