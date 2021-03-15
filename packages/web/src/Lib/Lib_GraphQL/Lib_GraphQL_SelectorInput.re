let toCache = {
  let rec rangeSelectorInputToCache = s =>
    Js.Dict.fromList([
      (
        "startSelector",
        s##startSelector
        ->selectorInputToCache
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      (
        "endSelector",
        s##endSelector
        ->selectorInputToCache
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      ("__typename", "RangeSelector"->Js.Json.string),
      ("type_", "RANGE_SELECTOR"->Js.Json.string),
    ])
    ->Js.Json.object_
  and xPathSelectorInputToCache = s =>
    Js.Dict.fromList([
      ("value", s##value->Js.Json.string),
      (
        "refinedBy",
        s##refinedBy
        ->Belt.Option.map(r =>
            r->Belt.Array.keepMap(selectorInputToCache)->Js.Json.array
          )
        ->Belt.Option.getWithDefault(Js.Json.null),
      ),
      ("__typename", "XPathSelector"->Js.Json.string),
      ("type_", "XPATH_SELECTOR"->Js.Json.string),
    ])
    ->Js.Json.object_

  and textPositionSelectorInputToCache = s =>
    Js.Dict.fromList([
      ("end_", s##end_->float_of_int->Js.Json.number),
      ("start", s##start->float_of_int->Js.Json.number),
      ("__typename", "TextPositionSelector"->Js.Json.string),
      ("type_", "TEXT_POSITION_SELECTOR"->Js.Json.string),
    ])
    ->Js.Json.object_
  and selectorInputToCache = s =>
    switch (s##xPathSelector, s##textPositionSelector, s##rangeSelector) {
    | (Some(s), _, _) => s->xPathSelectorInputToCache->Js.Option.some
    | (_, Some(s), _) => s->textPositionSelectorInputToCache->Js.Option.some
    | (_, _, Some(s)) => s->rangeSelectorInputToCache->Js.Option.some
    | s =>
      Js.log2("Unable to parse selectorInput.", s);
      None;
    };

  selectorInputToCache;
};
