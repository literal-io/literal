module Buffer = {
  [@bs.send]
  external toStringWithEncoding: (Node.Buffer.t, string) => string =
    "toString";
};

[@bs.val] external dirname: string = "__dirname";
