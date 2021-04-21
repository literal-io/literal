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

  module AuthSignInResult = {
    [@decco]
    type tokens = {
      idToken: string,
      refreshToken: string,
      accessToken: string,
    };

    [@decco]
    type t = {
      tokens: option(tokens),
      error: option(string),
    };

    let decode = t_decode;
  };

  module AuthGetUserInfoResult = {
    [@decco]
    type attributes = {
      email: string,
      [@decco.key "email_verified"]
      emailVerified: string,
      identities: option(string),
      sub: string,
    };

    [@decco]
    type t = {
      id: string,
      username: string,
      attributes,
    };

    let decode = t_decode;
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

  let make = (~type_, ~data=?, ()) => {type_, pid: Uuid.makeV4(), data};

  let encode = t_encode;
  let decode = t_decode;
};

let isWebview = () =>
  LiteralWebview.inst
  ->Belt.Option.map(LiteralWebview.isWebview)
  ->Belt.Option.getWithDefault(false);

let port: ref(option(MessagePort.t)) = ref(None);

/** Used for broadcasting events we've received across the WebView via Hub. **/
module HubEvent = {
  type t = {
    event: string,
    message: option(string),
    data: option(Js.Json.t),
  };

  let publish = (~event, ~message=?, ~data=?, ()) => {
    let event = AwsAmplify.Hub.{event, message, data};
    AwsAmplify.Hub.(dispatch(inst, "webview", event));
  };
};

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

  let handleGetTokensResult = (type_, event: option(Js.Json.t)) => {
    event->Belt.Option.forEach(data => {
      switch (WebEvent.authGetTokensResult_decode(data)) {
      | Belt.Result.Ok(_) => HubEvent.publish(~event=type_, ())
      | Belt.Result.Error(e) => Js.log2("handleGetTokensResult error", e)
      }
    });
  };

  let config = [|
    ("ROUTER_REPLACE", handleRouterReplace),
    (
      "AUTH_SIGN_IN_GOOGLE_RESULT",
      handleGetTokensResult("AUTH_SIGN_IN_GOOGLE_RESULT"),
    ),
  |];

  let dispatch = (ev: WebEvent.t) => {
    config
    ->Belt.Array.getBy(((type_, _)) => type_ === ev.type_)
    ->Belt.Option.map(((_, handler)) => handler(ev.data));
  };

  let register = callback => {
    let _ = Js.Array2.push(config, callback);
    ();
  };

  let unregister = type_ => {
    let idx =
      Js.Array2.findIndex(config, ((listenerType, _)) =>
        type_ === listenerType
      );
    if (idx !== (-1)) {
      let _ = Js.Array2.spliceInPlace(config, ~pos=idx, ~remove=1, ~add=[||]);
      ();
    };
  };
};

/** Messages to be dispatched to native once we establish a connection. */
let pendingMessageQueue = ref([||]);

let postMessage = webEvent => {
  switch (port^) {
  | Some(port) =>
    port->MessagePort.postMessage(
      webEvent->WebEvent.encode->Js.Json.stringify,
    );
    true;
  | None when isWebview() =>
    let _ = Js.Array.push(webEvent, pendingMessageQueue^);
    true;
  | None =>
    Js.log2("Attempted to postMessage, but found no MessagePort", webEvent);
    false;
  };
};

let postMessageForResult = (webEvent: WebEvent.t) => {
  Js.Promise.make((~resolve, ~reject) => {
    // infer type of result
    let resultType = webEvent.type_ ++ "_RESULT";
    let eventHandler = (
      resultType,
      data => {
        let _ = resolve(. data);
        let _ = WebEventHandler.unregister(resultType);
        ();
      },
    );
    let _ = WebEventHandler.register(eventHandler);
    let _ = postMessage(webEvent);
    ();
  });
};

let initialize = () => {
  let _ =
    Webapi.Dom.(
      window
      |> Webapi.Dom.Window.addEventListener("message", ev => {
           let message = ev->MessageEvent.unsafeOfEvent;
           /**
            * When page is loaded, WebView will postMessage with the
            * MessagePort to use for communication.
            */
           let _ =
             switch (port^) {
             | None when message->MessageEvent.portsGet->Js.Array.length > 0 =>
               port := message->MessageEvent.portsGet->Belt.Array.get(0);
               /** Drain pending message queue if one exists */
               let _ =
                 (pendingMessageQueue^)
                 ->Belt.Array.forEach(ev => {
                     let _ = postMessage(ev);
                     ();
                   });
               pendingMessageQueue := [||];
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
