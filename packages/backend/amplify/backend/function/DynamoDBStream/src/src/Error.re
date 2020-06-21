exception DeccoDecodeError(Decco.decodeError);
exception EventHandlerError(string);

let report = exn => {
  let _ = Js.Console.trace();
  let _ = Js.Console.log(exn);
};
