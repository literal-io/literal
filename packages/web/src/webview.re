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
    )
  | None =>
    Js.log2("Attempted to postMessage, but found no MessagePort", webEvent)
  };
