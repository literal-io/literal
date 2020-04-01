type t;
type client = (Node.Buffer.t, string) => t;

type subclassOptions = {imageMagick: bool};

[@bs.module] external _client: client = "gm";
[@bs.send]
external _subClass: (client, subclassOptions) => client = "subClass";

// hardcode the imageMagick client, as gm isn't available
let client = _client->_subClass({imageMagick: true});

exception SizeError;
type size = {
  width: int,
  height: int,
};
[@bs.send]
external _size: (t, (option(Js.Exn.t), size) => unit) => t = "size";
let size = t =>
  Js.Promise.make((~resolve, ~reject) => {
    let _ =
      _size(t, (err, size) => {
        switch (err) {
        | Some(_) => reject(. SizeError)
        | None => resolve(. size)
        }
      });
    ();
  });

// width, height, x, y
[@bs.send] external crop: (t, int, int, int, int) => t = "crop";

exception ToBufferError;
[@bs.send]
external _toBuffer:
  (t, string, (option(Js.Exn.t), Node.Buffer.t) => unit) => unit =
  "toBuffer";
let toBuffer = (t, format) =>
  Js.Promise.make((~resolve, ~reject) => {
    let _ =
      _toBuffer(t, format, (err, buf) => {
        switch (err) {
        | Some(_) => reject(. ToBufferError)
        | None => resolve(. buf)
        }
      });
    ();
  });
