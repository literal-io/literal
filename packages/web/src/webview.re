module MessagePort = {
  type t;
  [@bs.send] external postMessage: (t, string) => unit = "postMessage";
};

module MessageEvent = {
  [@bs.deriving abstract]
  type t =
    pri {
      origin: string,
      data: string,
      [@bs.as "type"]
      type_: string,
      ports: array(MessagePort.t),
    };

  include Webapi.Dom.Event.Impl({
    type nonrec t = t;
  });

  external unsafeOfEvent: Dom.event => t = "%identity";
};

module WebEvent = {
  [@decco]
  type routerReplace = {url: string};

  [@decco]
  type authGetTokensResult = {
    idToken: string,
    refreshToken: string,
    accessToken: string,
  };

  /**
   * See the following for class definition in Android:
   * android/app/src/main/java/io/literal/lib/WebEvent.java
   */
  [@decco]
  type t = {
    [@decco.key "type"]
    type_: string,
    pid: string,
    data: option(Js.Json.t),
  };

  let make = (~type_) => {type_, pid: Uuid.makeV4(), data: None};

  let encode = t_encode;
  let decode = t_decode;
};

let port: ref(option(MessagePort.t)) = ref(None);

/**
let handleSignInResult = event =>
  switch (WebEvent.signInResult_decode(event)) {
  | Belt.Result.Ok({refreshToken, accessToken, idToken}) =>
    let _ =
      Authentication.signInWithToken(idToken, refreshToken, accessToken)
      |> Js.Promise.then_(_ => {
           Next.Router.replace("/notes");
           Js.Promise.resolve();
         });
    ();
  | _ => ()
  };
**/
module WebEventHandler = {
  let handleRouterReplace = (event: option(Js.Json.t)) => {
    let _ =
      event->Belt.Option.map(data => {
        switch (WebEvent.routerReplace_decode(data)) {
        | Belt.Result.Ok({url}) =>
          Next.Router.replace(url);
          ();
        | _ => ()
        }
      });
    ();
  };

  let config = [|("ROUTER_REPLACE", handleRouterReplace)|];

  let dispatch = (ev: WebEvent.t) => {
    Js.log2("dispatch handling message", ev);
    config
    ->Belt.Array.getBy(((type_, _)) => type_ === ev.type_)
    ->Belt.Option.map(((_, handler)) => handler(ev.data));
  }

  let register = callback => {
    let _ = Js.Array2.push(config, callback);
    ();
  };

  let unregister = type_ => {/** FIXME: todo */};
};

let initialize = () => {
  let _ =
    Webapi.Dom.(
      window
      |> Webapi.Dom.Window.addEventListener("message", ev => {
           let message = ev->MessageEvent.unsafeOfEvent;
           Js.log2("webview message", ev);

           /**
            * When page is loaded, WebView will postMessage with the
            * MessagePort to use for communication.
            */
           let _ =
             switch (port^) {
             | None when message->MessageEvent.portsGet->Js.Array.length > 0 =>
               port := message->MessageEvent.portsGet->Belt.Array.get(0)
             | _ => ()
             };

           let json =
             try(message->MessageEvent.dataGet->Js.Json.parseExn) {
             | _ => Js.Json.null
             };

           let _ =
             json->WebEvent.decode->Belt.Result.map(WebEventHandler.dispatch);

           ();
         })
    );
  ();
};

let postMessage = webEvent =>
  switch (port^) {
  | Some(port) =>
    port->MessagePort.postMessage(
      webEvent->WebEvent.encode->Js.Json.stringify,
    );
    true;
  | None =>
    Js.log2("Attempted to postMessage, but found no MessagePort", webEvent);
    false;
  };

let postMessageForResult = (webEvent: WebEvent.t) =>
  Js.Promise.make((~resolve, ~reject) => {
    // infer type of result
    Js.log2("postMessageForResult begin", webEvent);
    let _ = Js.Global.setTimeout(
      () => {
        let resultType = webEvent.type_ ++ "_RESULT";
        let eventHandler = (
          resultType,
          data => {
            Js.log2("postMessageForResult", data);
            let _ = resolve(. data);
            let _ = WebEventHandler.unregister(resultType);
            ();
          },
        );
        let _ = WebEventHandler.register(eventHandler);
        let _ = postMessage(webEvent);
        ();
      },
      5000,
    )
  });
