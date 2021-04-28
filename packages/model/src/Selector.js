// Generated by BUCKLESCRIPT, PLEASE EDIT WITH CARE
'use strict';

var Curry = require("bs-platform/lib/js/curry.js");
var Decco = require("decco/src/Decco.js");
var Js_dict = require("bs-platform/lib/js/js_dict.js");
var Js_json = require("bs-platform/lib/js/js_json.js");
var Belt_Array = require("bs-platform/lib/js/belt_Array.js");
var Belt_Option = require("bs-platform/lib/js/belt_Option.js");

function rangeSelector_encode(v) {
  return Js_dict.fromArray([
              [
                "startSelector",
                t_encode(v.startSelector)
              ],
              [
                "endSelector",
                t_encode(v.endSelector)
              ],
              [
                "type",
                Decco.stringToJson(v.type_)
              ],
              [
                "__typename",
                Decco.stringToJson(v.typename)
              ]
            ]);
}

function rangeSelector_decode(v) {
  var dict = Js_json.classify(v);
  if (typeof dict === "number") {
    return Decco.error(undefined, "Not an object", v);
  }
  if (dict.TAG !== /* JSONObject */2) {
    return Decco.error(undefined, "Not an object", v);
  }
  var dict$1 = dict._0;
  var startSelector = t_decode(Belt_Option.getWithDefault(Js_dict.get(dict$1, "startSelector"), null));
  if (startSelector.TAG) {
    var e = startSelector._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".startSelector" + e.path,
              message: e.message,
              value: e.value
            }
          };
  }
  var endSelector = t_decode(Belt_Option.getWithDefault(Js_dict.get(dict$1, "endSelector"), null));
  if (endSelector.TAG) {
    var e$1 = endSelector._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".endSelector" + e$1.path,
              message: e$1.message,
              value: e$1.value
            }
          };
  }
  var type_ = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "type"), null));
  if (type_.TAG) {
    var e$2 = type_._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".type" + e$2.path,
              message: e$2.message,
              value: e$2.value
            }
          };
  }
  var typename = Belt_Option.getWithDefault(Belt_Option.map(Js_dict.get(dict$1, "__typename"), Decco.stringFromJson), {
        TAG: /* Ok */0,
        _0: "RangeSelector"
      });
  if (!typename.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              startSelector: startSelector._0,
              endSelector: endSelector._0,
              type_: type_._0,
              typename: typename._0
            }
          };
  }
  var e$3 = typename._0;
  return {
          TAG: /* Error */1,
          _0: {
            path: ".__typename" + e$3.path,
            message: e$3.message,
            value: e$3.value
          }
        };
}

function xPathSelector_encode(v) {
  return Js_dict.fromArray([
              [
                "value",
                Decco.stringToJson(v.value)
              ],
              [
                "refinedBy",
                Decco.optionToJson((function (param) {
                        return Decco.arrayToJson(t_encode, param);
                      }), v.refinedBy)
              ],
              [
                "type",
                Decco.stringToJson(v.type_)
              ],
              [
                "__typename",
                Decco.stringToJson(v.typename)
              ]
            ]);
}

function xPathSelector_decode(v) {
  var dict = Js_json.classify(v);
  if (typeof dict === "number") {
    return Decco.error(undefined, "Not an object", v);
  }
  if (dict.TAG !== /* JSONObject */2) {
    return Decco.error(undefined, "Not an object", v);
  }
  var dict$1 = dict._0;
  var value = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "value"), null));
  if (value.TAG) {
    var e = value._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".value" + e.path,
              message: e.message,
              value: e.value
            }
          };
  }
  var refinedBy = Decco.optionFromJson((function (param) {
          return Decco.arrayFromJson(t_decode, param);
        }), Belt_Option.getWithDefault(Js_dict.get(dict$1, "refinedBy"), null));
  if (refinedBy.TAG) {
    var e$1 = refinedBy._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".refinedBy" + e$1.path,
              message: e$1.message,
              value: e$1.value
            }
          };
  }
  var type_ = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "type"), null));
  if (type_.TAG) {
    var e$2 = type_._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".type" + e$2.path,
              message: e$2.message,
              value: e$2.value
            }
          };
  }
  var typename = Belt_Option.getWithDefault(Belt_Option.map(Js_dict.get(dict$1, "__typename"), Decco.stringFromJson), {
        TAG: /* Ok */0,
        _0: "XPathSelector"
      });
  if (!typename.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              value: value._0,
              refinedBy: refinedBy._0,
              type_: type_._0,
              typename: typename._0
            }
          };
  }
  var e$3 = typename._0;
  return {
          TAG: /* Error */1,
          _0: {
            path: ".__typename" + e$3.path,
            message: e$3.message,
            value: e$3.value
          }
        };
}

function textPositionSelector_encode(v) {
  return Js_dict.fromArray([
              [
                "end",
                Decco.intToJson(v.end_)
              ],
              [
                "start",
                Decco.intToJson(v.start)
              ],
              [
                "type",
                Decco.stringToJson(v.type_)
              ],
              [
                "__typename",
                Decco.stringToJson(v.typename)
              ]
            ]);
}

function textPositionSelector_decode(v) {
  var dict = Js_json.classify(v);
  if (typeof dict === "number") {
    return Decco.error(undefined, "Not an object", v);
  }
  if (dict.TAG !== /* JSONObject */2) {
    return Decco.error(undefined, "Not an object", v);
  }
  var dict$1 = dict._0;
  var end_ = Decco.intFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "end"), null));
  if (end_.TAG) {
    var e = end_._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".end" + e.path,
              message: e.message,
              value: e.value
            }
          };
  }
  var start = Decco.intFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "start"), null));
  if (start.TAG) {
    var e$1 = start._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".start" + e$1.path,
              message: e$1.message,
              value: e$1.value
            }
          };
  }
  var type_ = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict$1, "type"), null));
  if (type_.TAG) {
    var e$2 = type_._0;
    return {
            TAG: /* Error */1,
            _0: {
              path: ".type" + e$2.path,
              message: e$2.message,
              value: e$2.value
            }
          };
  }
  var typename = Belt_Option.getWithDefault(Belt_Option.map(Js_dict.get(dict$1, "__typename"), Decco.stringFromJson), {
        TAG: /* Ok */0,
        _0: "TextPositionSelector"
      });
  if (!typename.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              end_: end_._0,
              start: start._0,
              type_: type_._0,
              typename: typename._0
            }
          };
  }
  var e$3 = typename._0;
  return {
          TAG: /* Error */1,
          _0: {
            path: ".__typename" + e$3.path,
            message: e$3.message,
            value: e$3.value
          }
        };
}

function t_encode(v) {
  switch (v.TAG | 0) {
    case /* RangeSelector */0 :
        return [
                "RangeSelector",
                rangeSelector_encode(v._0)
              ];
    case /* XPathSelector */1 :
        return [
                "XPathSelector",
                xPathSelector_encode(v._0)
              ];
    case /* TextPositionSelector */2 :
        return [
                "TextPositionSelector",
                textPositionSelector_encode(v._0)
              ];
    case /* NotImplemented_Passthrough */3 :
        return [
                "NotImplemented_Passthrough",
                v._0
              ];
    
  }
}

function t_decode(v) {
  var jsonArr = Js_json.classify(v);
  if (typeof jsonArr === "number") {
    return Decco.error(undefined, "Not a variant", v);
  }
  if (jsonArr.TAG !== /* JSONArray */3) {
    return Decco.error(undefined, "Not a variant", v);
  }
  var jsonArr$1 = jsonArr._0;
  if (jsonArr$1.length === 0) {
    return Decco.error(undefined, "Expected variant, found empty array", v);
  }
  var tagged = jsonArr$1.map(Js_json.classify);
  var match = Belt_Array.getExn(tagged, 0);
  if (typeof match !== "number" && !match.TAG) {
    switch (match._0) {
      case "NotImplemented_Passthrough" :
          if (tagged.length !== 2) {
            return Decco.error(undefined, "Invalid number of arguments to variant constructor", v);
          }
          var v0 = {
            TAG: /* Ok */0,
            _0: Belt_Array.getExn(jsonArr$1, 1)
          };
          if (!v0.TAG) {
            return {
                    TAG: /* Ok */0,
                    _0: {
                      TAG: /* NotImplemented_Passthrough */3,
                      _0: v0._0
                    }
                  };
          }
          var e = v0._0;
          return {
                  TAG: /* Error */1,
                  _0: {
                    path: "[0]" + e.path,
                    message: e.message,
                    value: e.value
                  }
                };
      case "RangeSelector" :
          if (tagged.length !== 2) {
            return Decco.error(undefined, "Invalid number of arguments to variant constructor", v);
          }
          var v0$1 = rangeSelector_decode(Belt_Array.getExn(jsonArr$1, 1));
          if (!v0$1.TAG) {
            return {
                    TAG: /* Ok */0,
                    _0: {
                      TAG: /* RangeSelector */0,
                      _0: v0$1._0
                    }
                  };
          }
          var e$1 = v0$1._0;
          return {
                  TAG: /* Error */1,
                  _0: {
                    path: "[0]" + e$1.path,
                    message: e$1.message,
                    value: e$1.value
                  }
                };
      case "TextPositionSelector" :
          if (tagged.length !== 2) {
            return Decco.error(undefined, "Invalid number of arguments to variant constructor", v);
          }
          var v0$2 = textPositionSelector_decode(Belt_Array.getExn(jsonArr$1, 1));
          if (!v0$2.TAG) {
            return {
                    TAG: /* Ok */0,
                    _0: {
                      TAG: /* TextPositionSelector */2,
                      _0: v0$2._0
                    }
                  };
          }
          var e$2 = v0$2._0;
          return {
                  TAG: /* Error */1,
                  _0: {
                    path: "[0]" + e$2.path,
                    message: e$2.message,
                    value: e$2.value
                  }
                };
      case "XPathSelector" :
          if (tagged.length !== 2) {
            return Decco.error(undefined, "Invalid number of arguments to variant constructor", v);
          }
          var v0$3 = xPathSelector_decode(Belt_Array.getExn(jsonArr$1, 1));
          if (!v0$3.TAG) {
            return {
                    TAG: /* Ok */0,
                    _0: {
                      TAG: /* XPathSelector */1,
                      _0: v0$3._0
                    }
                  };
          }
          var e$3 = v0$3._0;
          return {
                  TAG: /* Error */1,
                  _0: {
                    path: "[0]" + e$3.path,
                    message: e$3.message,
                    value: e$3.value
                  }
                };
      default:
        
    }
  }
  return Decco.error(undefined, "Invalid variant constructor", Belt_Array.getExn(jsonArr$1, 0));
}

function makeTextPositionSelector(end_, start, type_, param) {
  return {
          end_: end_,
          start: start,
          type_: type_,
          typename: "TextPositionSelector"
        };
}

function makeTextPositionSelectorFromGraphQL(textPositionSelector) {
  return {
          TAG: /* TextPositionSelector */2,
          _0: makeTextPositionSelector(textPositionSelector.end_, textPositionSelector.start, "TEXT_POSITION_SELECTOR", undefined)
        };
}

function makeXPathSelector(value, refinedBy, type_, param) {
  return {
          value: value,
          refinedBy: refinedBy,
          type_: type_,
          typename: "XPathSelector"
        };
}

function makeXPathSelectorFromGraphQL(makeRefinedBy, xPathSelector) {
  return {
          TAG: /* XPathSelector */1,
          _0: makeXPathSelector(xPathSelector.value, Belt_Option.map(xPathSelector.refinedBy, (function (r) {
                      return Belt_Array.keepMap(r, makeRefinedBy);
                    })), "XPATH_SELECTOR", undefined)
        };
}

function makeRangeSelector(startSelector, endSelector, type_, param) {
  return {
          startSelector: startSelector,
          endSelector: endSelector,
          type_: type_,
          typename: "RangeSelector"
        };
}

function makeRangeSelectorFromGraphQL(makeStartSelector, makeEndSelector, rangeSelector) {
  var match = Curry._1(makeStartSelector, rangeSelector.startSelector);
  var match$1 = Curry._1(makeEndSelector, rangeSelector.endSelector);
  if (match !== undefined && match$1 !== undefined) {
    return {
            TAG: /* RangeSelector */0,
            _0: makeRangeSelector(match, match$1, "RANGE_SELECTOR", undefined)
          };
  }
  
}

function t_decode$1(json) {
  var match = Js_json.classify(json);
  if (typeof match === "number") {
    return {
            TAG: /* Error */1,
            _0: {
              path: "",
              message: "Expected JSONObject for selector.",
              value: json
            }
          };
  }
  if (match.TAG !== /* JSONObject */2) {
    return {
            TAG: /* Error */1,
            _0: {
              path: "",
              message: "Expected JSONObject for selector.",
              value: json
            }
          };
  }
  var rangeSelector = rangeSelector_decode(json);
  if (!rangeSelector.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              TAG: /* RangeSelector */0,
              _0: rangeSelector._0
            }
          };
  }
  var xPathSelector = xPathSelector_decode(json);
  if (!xPathSelector.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              TAG: /* XPathSelector */1,
              _0: xPathSelector._0
            }
          };
  }
  var textPositionSelector = textPositionSelector_decode(json);
  if (textPositionSelector.TAG) {
    return {
            TAG: /* Ok */0,
            _0: {
              TAG: /* NotImplemented_Passthrough */3,
              _0: json
            }
          };
  } else {
    return {
            TAG: /* Ok */0,
            _0: {
              TAG: /* TextPositionSelector */2,
              _0: textPositionSelector._0
            }
          };
  }
}

function t_encode$1(inst) {
  switch (inst.TAG | 0) {
    case /* RangeSelector */0 :
        return rangeSelector_encode(inst._0);
    case /* XPathSelector */1 :
        return xPathSelector_encode(inst._0);
    case /* TextPositionSelector */2 :
        return textPositionSelector_encode(inst._0);
    case /* NotImplemented_Passthrough */3 :
        return inst._0;
    
  }
}

t_encode = t_encode$1; t_decode = t_decode$1
;

var codec = [
  t_encode$1,
  t_decode$1
];

exports.rangeSelector_encode = rangeSelector_encode;
exports.rangeSelector_decode = rangeSelector_decode;
exports.xPathSelector_encode = xPathSelector_encode;
exports.xPathSelector_decode = xPathSelector_decode;
exports.textPositionSelector_encode = textPositionSelector_encode;
exports.textPositionSelector_decode = textPositionSelector_decode;
exports.makeTextPositionSelector = makeTextPositionSelector;
exports.makeTextPositionSelectorFromGraphQL = makeTextPositionSelectorFromGraphQL;
exports.makeXPathSelector = makeXPathSelector;
exports.makeXPathSelectorFromGraphQL = makeXPathSelectorFromGraphQL;
exports.makeRangeSelector = makeRangeSelector;
exports.makeRangeSelectorFromGraphQL = makeRangeSelectorFromGraphQL;
exports.t_decode = t_decode$1;
exports.t_encode = t_encode$1;
exports.codec = codec;
/*  Not a pure module */
