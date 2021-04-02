let make = (~timeState=?, ~httpRequestState=?, ()) => {
  "timeState": timeState,
  "httpRequestState": httpRequestState,
};
let makeTimeStateInput =
    (
      ~sourceDate=?,
      ~cached=?,
      ~sourceDateStart=?,
      ~sourceDateEnd=?,
      ~refinedBy=?,
      (),
    ) => {
  "sourceDate": sourceDate,
  "cached": cached,
  "sourceDateStart": sourceDateStart,
  "sourceDateEnd": sourceDateEnd,
  "refinedBy": refinedBy,
  "type": `TIME_STATE,
};

let toCache = s => {
  let timeStateInputToCache = s =>
    Js.Dict.fromList([
      (
        "sourceDate",
        s##sourceDate
        ->Belt.Option.map(s => s->Js.Json.array)
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "cached",
        s##cached
        ->Belt.Option.map(s =>
            s->Belt.Array.map(Js.Json.string)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      ("__typename", "TimeState"->Js.Json.string),
      ("type_", "TIME_STATE"->Js.Json.string),
    ])
    ->Js.Json.object_;

  switch (s##timeState) {
  | Some(timeState) => timeStateInputToCache(timeState)->Js.Option.some
  | None =>
    let _ =
      Error.(
        report(
          GenericErrorWithExtra((
            "Unable to parse StateInput",
            Js.Dict.fromList([
              (
                "state",
                Js.Json.stringifyAny(s)
                ->Belt.Option.getWithDefault("Unable to stringify state")
                ->Js.Json.string,
              ),
            ])
            ->Js.Json.object_,
          )),
        )
      );
    None;
  };
};
