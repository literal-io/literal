// Generated by BUCKLESCRIPT, PLEASE EDIT WITH CARE
'use strict';

var Js_exn = require("bs-platform/lib/js/js_exn.js");
var Core = require("@aws-amplify/core");
var Lib_Lambda$GraphQLResolver = require("./Lib_Lambda.bs.js");
var Lib_Constants$GraphQLResolver = require("./Lib_Constants.bs.js");
var QueryResolver_CreateHighlightFromScreenshot$GraphQLResolver = require("./QueryResolver_CreateHighlightFromScreenshot.bs.js");

((require('isomorphic-fetch')));

Core.default.configure(Lib_Constants$GraphQLResolver.awsAmplifyConfig);

function handler($$event) {
  console.log(JSON.stringify($$event));
  var match = Lib_Lambda$GraphQLResolver.event_decode($$event);
  if (match.tag) {
    console.log("Unable to decode event.");
    return Js_exn.raiseError(match[0].message);
  } else {
    var ctx = match[0];
    if (ctx.typeName === "Mutation" && ctx.fieldName === "createHighlightFromScreenshot") {
      return QueryResolver_CreateHighlightFromScreenshot$GraphQLResolver.resolver(ctx);
    } else {
      console.log("Resolver not found.");
      return Js_exn.raiseError("Resolver not found.");
    }
  }
}

exports.handler = handler;
/*  Not a pure module */
