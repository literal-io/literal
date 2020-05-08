let merge = [%raw {|
  function(a, b) { return {...a, ...b} }
|}];
