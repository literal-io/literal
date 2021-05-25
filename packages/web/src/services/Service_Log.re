let log = (tag, message) =>
  if (Constants.Env.nodeEnv != "production") {
    Js.log2("[" ++ tag ++ "]", message);
  };

let log2 = (tag, message1, message2) =>
  if (Constants.Env.nodeEnv != "production") {
    Js.log3("[" ++ tag ++ "]", message1, message2);
  };
