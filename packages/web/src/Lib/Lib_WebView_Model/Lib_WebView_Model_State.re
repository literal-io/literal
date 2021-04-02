[@decco]
type timeState = {
  cached: option(array(string)),
  sourceDate: option(array(Js.Json.t)),
  [@decco.default "TIME_STATE"] [@decco.key "type"]
  type_: string,
  [@decco.default "TimeState"] [@decco.key "__typename"]
  typename: string,
};

type t =
  | TimeState(timeState)
  | NotImplemented_Passthrough(Js.Json.t);

let makeTimeState = (~cached, ~sourceDate, ()) => {
  cached,
  sourceDate,
  type_: "TIME_STATE",
  typename: "TimeState",
};

let makeTimeStateFromGraphQL = timeState =>
  TimeState(
    makeTimeState(
      ~cached=timeState##cached,
      ~sourceDate=timeState##sourceDate,
      (),
    ),
  );

let t_decode = json =>
  switch (json->Js.Json.classify) {
  | JSONObject(_) =>
    switch (timeState_decode(json)) {
    | Ok(timeState) => Ok(TimeState(timeState))
    | _ => Ok(NotImplemented_Passthrough(json))
    }
  | _ =>
    Error({
      Decco.path: "",
      message: "Expected JSONObject for state.",
      value: json,
    })
  };

let t_encode =
  fun
  | TimeState(t) => timeState_encode(t)
  | NotImplemented_Passthrough(t) => t;
