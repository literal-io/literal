[@decco]
type rangeSelector = {
  startSelector: t,
  endSelector: t,
  [@decco.key "type"]
  type_: string,
  [@decco.default "RangeSelector"] [@decco.key "__typename"]
  typename: string,
}
[@decco]
and xPathSelector = {
  value: string,
  refinedBy: option(array(t)),
  [@decco.key "type"]
  type_: string,
  [@decco.default "XPathSelector"] [@decco.key "__typename"]
  typename: string,
}
[@decco]
and textPositionSelector = {
  [@decco.key "end"]
  end_: int,
  start: int,
  [@decco.key "type"]
  type_: string,
  [@decco.default "TextPositionSelector"] [@decco.key "__typename"]
  typename: string,
}
[@decco]
and t =
  | RangeSelector(rangeSelector)
  | XPathSelector(xPathSelector)
  | TextPositionSelector(textPositionSelector)
  | NotImplemented_Passthrough(Js.Json.t);

let makeTextPositionSelector = (~end_, ~start, ~type_, ()) => {
  end_,
  start,
  type_,
  typename: "TextPositionSelector",
};

let makeTextPositionSelectorFromGraphQL = textPositionSelector =>
  TextPositionSelector(
    makeTextPositionSelector(
      ~start=textPositionSelector##start,
      ~end_=textPositionSelector##end_,
      ~type_="TEXT_POSITION_SELECTOR",
      (),
    ),
  );

let makeXPathSelector = (~value, ~refinedBy=?, ~type_, ()) => {
  value,
  refinedBy,
  type_,
  typename: "XPathSelector",
};

let makeXPathSelectorFromGraphQL = (~makeRefinedBy, xPathSelector) =>
  XPathSelector(
    makeXPathSelector(
      ~value=xPathSelector##value,
      ~refinedBy=?
        xPathSelector##refinedBy
        ->Belt.Option.map(r => r->Belt.Array.keepMap(makeRefinedBy)),
      ~type_="XPATH_SELECTOR",
      (),
    ),
  );

let makeRangeSelector = (~startSelector, ~endSelector, ~type_, ()) => {
  startSelector,
  endSelector,
  type_,
  typename: "RangeSelector",
};

let makeRangeSelectorFromGraphQL =
    (~makeStartSelector, ~makeEndSelector, rangeSelector) => {
  switch (
    makeStartSelector(rangeSelector##startSelector),
    makeEndSelector(rangeSelector##endSelector),
  ) {
  | (Some(startSelector), Some(endSelector)) =>
    Some(
      RangeSelector(
        makeRangeSelector(
          ~startSelector,
          ~endSelector,
          ~type_="RANGE_SELECTOR",
          (),
        ),
      ),
    )
  | _ => None
  };
};

let t_decode = json =>
  switch (json->Js.Json.classify) {
  | JSONObject(_) =>
    switch (rangeSelector_decode(json)) {
    | Ok(rangeSelector) => Ok(RangeSelector(rangeSelector))
    | _ =>
      switch (xPathSelector_decode(json)) {
      | Ok(xPathSelector) => Ok(XPathSelector(xPathSelector))
      | _ =>
        switch (textPositionSelector_decode(json)) {
        | Ok(textPositionSelector) =>
          Ok(TextPositionSelector(textPositionSelector))
        | _ => Ok(NotImplemented_Passthrough(json))
        }
      }
    }
  | _ =>
    Error({
      Decco.path: "",
      message: "Expected JSONObject for selector.",
      value: json,
    })
  };

let t_encode = inst =>
  switch (inst) {
  | RangeSelector(target) => rangeSelector_encode(target)
  | XPathSelector(target) => xPathSelector_encode(target)
  | TextPositionSelector(target) => textPositionSelector_encode(target)
  | NotImplemented_Passthrough(json) => json
  };

%raw
/**
 * FIXME: This is a hack to override the codecs used by t, which seems to require the @decco annotation,
 * even though we want to use the custom variant encoder defined above (t_encode, t_decode)
 */
{| t_encode = t_encode$1; t_decode = t_decode$1 |};

let codec: Decco.codec(t) = (t_encode, t_decode);
