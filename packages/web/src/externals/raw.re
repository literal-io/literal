let merge = [%raw {|
  function(a, b) { return {...a, ...b} }
|}];

let unsafeEq = [%raw
  {| function(a, b) { return a === b } |}
];

let global = [%raw {|
  function () {
    if (typeof self !== 'undefined') { return self; }
    if (typeof window !== 'undefined') { return window; }
    if (typeof global !== 'undefined') { return global; }
    throw new Error('unable to locate global object');
  }
|}];

let isBrowser = [%raw {|
  function () {
    return typeof window !== 'undefined'
  }
|}];

let maybeScrollTo = [%raw {|
  function (elem, params) {
    if (elem.scrollTo) {
      elem.scrollTo(params)
    }
  }
|}];
