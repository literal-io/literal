// Generated by BUCKLESCRIPT, PLEASE EDIT WITH CARE
'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var Decco = require("decco/src/Decco.js");
var Js_exn = require("bs-platform/lib/js/js_exn.js");
var Js_dict = require("bs-platform/lib/js/js_dict.js");
var Js_json = require("bs-platform/lib/js/js_json.js");
var Js_option = require("bs-platform/lib/js/js_option.js");
var Belt_Option = require("bs-platform/lib/js/belt_Option.js");
var Belt_Result = require("bs-platform/lib/js/belt_Result.js");
var Caml_option = require("bs-platform/lib/js/caml_option.js");
var Api = require("@aws-amplify/api");
var $$Storage = require("@aws-amplify/storage");

var ppx_printed_query = "query GetScreenshot($screenshotId: ID!)  {\ngetScreenshot(id: $screenshotId)  {\nfile  {\nbucket  \nkey  \nregion  \n}\n\n}\n\n}\n";

function parse(value) {
  var value$1 = Js_option.getExn(Js_json.decodeObject(value));
  var match = Js_dict.get(value$1, "getScreenshot");
  var tmp;
  if (match !== undefined) {
    var value$2 = Caml_option.valFromOption(match);
    var match$1 = Js_json.decodeNull(value$2);
    if (match$1 !== undefined) {
      tmp = undefined;
    } else {
      var value$3 = Js_option.getExn(Js_json.decodeObject(value$2));
      var match$2 = Js_dict.get(value$3, "file");
      var tmp$1;
      if (match$2 !== undefined) {
        var value$4 = Js_option.getExn(Js_json.decodeObject(Caml_option.valFromOption(match$2)));
        var match$3 = Js_dict.get(value$4, "bucket");
        var tmp$2;
        if (match$3 !== undefined) {
          var value$5 = Caml_option.valFromOption(match$3);
          var match$4 = Js_json.decodeString(value$5);
          tmp$2 = match$4 !== undefined ? match$4 : Js_exn.raiseError("graphql_ppx: Expected string, got " + JSON.stringify(value$5));
        } else {
          tmp$2 = Js_exn.raiseError("graphql_ppx: Field bucket on type S3Object is missing");
        }
        var match$5 = Js_dict.get(value$4, "key");
        var tmp$3;
        if (match$5 !== undefined) {
          var value$6 = Caml_option.valFromOption(match$5);
          var match$6 = Js_json.decodeString(value$6);
          tmp$3 = match$6 !== undefined ? match$6 : Js_exn.raiseError("graphql_ppx: Expected string, got " + JSON.stringify(value$6));
        } else {
          tmp$3 = Js_exn.raiseError("graphql_ppx: Field key on type S3Object is missing");
        }
        var match$7 = Js_dict.get(value$4, "region");
        var tmp$4;
        if (match$7 !== undefined) {
          var value$7 = Caml_option.valFromOption(match$7);
          var match$8 = Js_json.decodeString(value$7);
          tmp$4 = match$8 !== undefined ? match$8 : Js_exn.raiseError("graphql_ppx: Expected string, got " + JSON.stringify(value$7));
        } else {
          tmp$4 = Js_exn.raiseError("graphql_ppx: Field region on type S3Object is missing");
        }
        tmp$1 = {
          bucket: tmp$2,
          key: tmp$3,
          region: tmp$4
        };
      } else {
        tmp$1 = Js_exn.raiseError("graphql_ppx: Field file on type Screenshot is missing");
      }
      tmp = {
        file: tmp$1
      };
    }
  } else {
    tmp = undefined;
  }
  return {
          getScreenshot: tmp
        };
}

function make(screenshotId, param) {
  return {
          query: ppx_printed_query,
          variables: Js_dict.fromArray([/* tuple */[
                    "screenshotId",
                    screenshotId
                  ]].filter((function (param) {
                      return !Js_json.test(param[1], /* Null */5);
                    }))),
          parse: parse
        };
}

function makeWithVariables(variables) {
  var screenshotId = variables.screenshotId;
  return {
          query: ppx_printed_query,
          variables: Js_dict.fromArray([/* tuple */[
                    "screenshotId",
                    screenshotId
                  ]].filter((function (param) {
                      return !Js_json.test(param[1], /* Null */5);
                    }))),
          parse: parse
        };
}

function makeVariables(screenshotId, param) {
  return Js_dict.fromArray([/* tuple */[
                  "screenshotId",
                  screenshotId
                ]].filter((function (param) {
                    return !Js_json.test(param[1], /* Null */5);
                  })));
}

function definition_002(graphql_ppx_use_json_variables_fn, screenshotId, param) {
  return Curry._1(graphql_ppx_use_json_variables_fn, Js_dict.fromArray([/* tuple */[
                      "screenshotId",
                      screenshotId
                    ]].filter((function (param) {
                        return !Js_json.test(param[1], /* Null */5);
                      }))));
}

var definition = /* tuple */[
  parse,
  ppx_printed_query,
  definition_002
];

function ret_type(f) {
  return { };
}

var MT_Ret = { };

var GetScreenshotQuery = {
  ppx_printed_query: ppx_printed_query,
  query: ppx_printed_query,
  parse: parse,
  make: make,
  makeWithVariables: makeWithVariables,
  makeVariables: makeVariables,
  definition: definition,
  ret_type: ret_type,
  MT_Ret: MT_Ret
};

function id(param) {
  return param.id;
}

function createdAt(param) {
  return param.createdAt;
}

function screenshotId(param) {
  return param.screenshotId;
}

function note(param) {
  return param.note;
}

function argumentsInput_encode(v) {
  return Js_dict.fromArray([
              /* tuple */[
                "id",
                Decco.optionToJson(Decco.stringToJson, v.id)
              ],
              /* tuple */[
                "createdAt",
                Decco.optionToJson(Decco.stringToJson, v.createdAt)
              ],
              /* tuple */[
                "screenshotId",
                Decco.stringToJson(v.screenshotId)
              ],
              /* tuple */[
                "note",
                Decco.stringToJson(v.note)
              ]
            ]);
}

function argumentsInput_decode(v) {
  var match = Js_json.classify(v);
  if (typeof match === "number" || match.tag !== /* JSONObject */2) {
    return Decco.error(undefined, "Not an object", v);
  } else {
    var dict = match[0];
    var match$1 = Decco.optionFromJson(Decco.stringFromJson, Belt_Option.getWithDefault(Js_dict.get(dict, "id"), null));
    if (match$1.tag) {
      var e = match$1[0];
      return /* Error */Block.__(1, [{
                  path: ".id" + e.path,
                  message: e.message,
                  value: e.value
                }]);
    } else {
      var match$2 = Decco.optionFromJson(Decco.stringFromJson, Belt_Option.getWithDefault(Js_dict.get(dict, "createdAt"), null));
      if (match$2.tag) {
        var e$1 = match$2[0];
        return /* Error */Block.__(1, [{
                    path: ".createdAt" + e$1.path,
                    message: e$1.message,
                    value: e$1.value
                  }]);
      } else {
        var match$3 = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict, "screenshotId"), null));
        if (match$3.tag) {
          var e$2 = match$3[0];
          return /* Error */Block.__(1, [{
                      path: ".screenshotId" + e$2.path,
                      message: e$2.message,
                      value: e$2.value
                    }]);
        } else {
          var match$4 = Decco.stringFromJson(Belt_Option.getWithDefault(Js_dict.get(dict, "note"), null));
          if (match$4.tag) {
            var e$3 = match$4[0];
            return /* Error */Block.__(1, [{
                        path: ".note" + e$3.path,
                        message: e$3.message,
                        value: e$3.value
                      }]);
          } else {
            return /* Ok */Block.__(0, [{
                        id: match$1[0],
                        createdAt: match$2[0],
                        screenshotId: match$3[0],
                        note: match$4[0]
                      }]);
          }
        }
      }
    }
  }
}

function input(param) {
  return param.input;
}

function arguments_encode(v) {
  return Js_dict.fromArray([/* tuple */[
                "input",
                argumentsInput_encode(v.input)
              ]]);
}

function arguments_decode(v) {
  var match = Js_json.classify(v);
  if (typeof match === "number" || match.tag !== /* JSONObject */2) {
    return Decco.error(undefined, "Not an object", v);
  } else {
    var match$1 = argumentsInput_decode(Belt_Option.getWithDefault(Js_dict.get(match[0], "input"), null));
    if (match$1.tag) {
      var e = match$1[0];
      return /* Error */Block.__(1, [{
                  path: ".input" + e.path,
                  message: e.message,
                  value: e.value
                }]);
    } else {
      return /* Ok */Block.__(0, [{
                  input: match$1[0]
                }]);
    }
  }
}

function resolver(ctx) {
  var match = Belt_Result.map(arguments_decode(ctx.arguments), input);
  if (match.tag) {
    console.log("Unable to decode arguments");
    return Js_exn.raiseError(match[0].message);
  } else {
    var query = make(match[0].screenshotId, /* () */0);
    var op = {
      query: query.query,
      variables: query.variables
    };
    return Api.default.graphql(op).then((function (r) {
                      var data = parse(r);
                      var match = data.getScreenshot;
                      if (match !== undefined) {
                        return $$Storage.default.get(Caml_option.valFromOption(match).file.key, {
                                      level: "public",
                                      download: true
                                    }).then((function (s) {
                                      return Promise.resolve(Caml_option.some(s.Data));
                                    }));
                      } else {
                        return Promise.resolve(undefined);
                      }
                    })).then((function (data) {
                    return Belt_Option.getWithDefault(Belt_Option.map(data, (function (data) {
                                      data.toString("base64");
                                      return Promise.resolve(/* () */0);
                                    })), Promise.resolve(undefined));
                  })).catch((function (err) {
                  return Promise.resolve(undefined);
                }));
  }
}

var CreateHighlightFromScreenshot = {
  id: id,
  createdAt: createdAt,
  screenshotId: screenshotId,
  note: note,
  argumentsInput_encode: argumentsInput_encode,
  argumentsInput_decode: argumentsInput_decode,
  input: input,
  arguments_encode: arguments_encode,
  arguments_decode: arguments_decode,
  resolver: resolver
};

exports.GetScreenshotQuery = GetScreenshotQuery;
exports.CreateHighlightFromScreenshot = CreateHighlightFromScreenshot;
/* @aws-amplify/api Not a pure module */
