type literalWebview;
[@bs.get] [@bs.return nullable]
external inst: Dom.window => option(literalWebview) = "literalWebview";

[@bs.send] external isWebview: literalWebview => bool = "isWebview";
[@bs.send]
external sendMessagePort: literalWebview => unit = "sendMessagePort";
