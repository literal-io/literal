exception DeccoDecodeError(Decco.decodeError);

let report = exn => {
  let _ = Js.Console.trace();
  let _ = Js.Console.log(exn);
};
