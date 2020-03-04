module CreateHighlightFromScreenshot = {
  [@decco]
  [@bs.deriving accessors]
  type argumentsInput = {
    id: option(string),
    createdAt: option(string),
    screenshotId: string,
    note: string,
  };

  [@decco]
  [@bs.deriving accessors]
  type arguments = {input: argumentsInput};

  let resolver = (ctx: Lib_Lambda.event) =>
    switch (ctx.arguments->arguments_decode->Belt.Result.map(input)) {
    | Belt.Result.Ok(input) => input.screenshotId
    | Belt.Result.Error(e) =>
      Js.log("Unable to decode arguments");
      Js.Exn.raiseError(e.message);
    };
};
