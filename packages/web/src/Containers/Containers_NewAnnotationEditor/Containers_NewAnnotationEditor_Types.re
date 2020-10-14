[@bs.deriving jsConverter]
type phase = [ | `PhasePrompt | `PhaseTextInput | `PhaseFileInput];
type phaseData = option(Webapi.File.t);
type phaseState = {
  phase,
  data: phaseData,
};

let phase_encode = p => p->phaseToJs->Js.Json.string;
let phase_decode = json =>
  switch (json->Js.Json.decodeString->Belt.Option.flatMap(phaseFromJs)) {
  | Some(p) => Ok(p)
  | None =>
    Error(Decco.{path: "", message: "Not a phase value.", value: json})
  };

