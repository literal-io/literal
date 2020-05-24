let thunkP = (~label, fn) => {
  let begin_ = Js.Date.now();
  fn()
  |> Js.Promise.then_(result => {
       let end_ = Js.Date.now();
       Js.log3("timing", label, Js.Float.toString(end_ -. begin_) ++ "ms");
       Js.Promise.resolve(result);
     });
};
