let merge = [%raw {|
  function(a, b) { return {...a, ...b} }
|}];

let unsafeEq = [%raw
  {| function(a, b) { return a === b } |}
];
