open Styles;

type direction =
  | Horizontal
  | Vertical;

module Container = {
  [@react.component]
  let make = (~direction, ~onIdxChange=?, ~initialIdx=?, ~children) => {
    let containerRef = React.useRef(Js.Nullable.null);
    let (hasScrolledToInitialIdx, setHasScrolledToInitialIdx) =
      React.useState(() =>
        switch (initialIdx) {
        | Some(idx) when idx === 0 => true
        | Some(_)
        | None => false
        }
      );
    let _ =
      React.useEffect0(() => {
        let _ =
          switch (initialIdx) {
          | Some(initialIdx) when initialIdx > 0 && !hasScrolledToInitialIdx =>
            let elem = containerRef->React.Ref.current->Js.Nullable.toOption;
            switch (elem, direction) {
            | (Some(elem), Horizontal) =>
              let width = Webapi.Dom.Element.clientWidth(elem);
              let _ =
                Webapi.Dom.Element.setScrollLeft(
                  elem,
                  float_of_int(width * initialIdx),
                );
              let _ = setHasScrolledToInitialIdx(_ => true);
              ();
            | (Some(elem), Vertical) =>
              let height = Webapi.Dom.Element.clientHeight(elem);
              let _ =
                Webapi.Dom.Element.setScrollTop(
                  elem,
                  float_of_int(height * initialIdx),
                );
              let _ = setHasScrolledToInitialIdx(_ => true);
              ();
            | _ => ()
            };
          | _ => ()
          };
        None;
      });

    let onScroll = ev => {
      let _ = ReactEvent.UI.stopPropagation(ev);
      let (scroll, containerDimen) =
        switch (direction) {
        | Vertical =>
          let scrollTop = ReactEvent.UI.target(ev)##scrollTop;
          let bodyHeight =
            Webapi.Dom.(
              document
              |> Document.unsafeAsHtmlDocument
              |> HtmlDocument.body
              |> Js.Option.getExn
              |> Element.clientHeight
            );
          (scrollTop, bodyHeight);
        | Horizontal =>
          let scrollLeft = ReactEvent.UI.target(ev)##scrollLeft;
          let windowWidth = Webapi.Dom.(window->Window.innerWidth);
          (scrollLeft, windowWidth);
        };
      let relativePos = scroll /. float_of_int(containerDimen);
      let activeIdx =
        switch (Constants.browser()->Bowser.getBrowserName) {
        | Some(`Chrome) => relativePos->Js.Math.round->int_of_float
        | Some(`Safari)
        | _ => relativePos->Js.Math.floor
        };
      let _ = onIdxChange->Belt.Option.map(fn => fn(activeIdx));
      ();
    };

    <div
      onScroll=?{hasScrolledToInitialIdx ? Some(onScroll) : None}
      ref={containerRef->ReactDOMRe.Ref.domRef}
      className={cn([
        "h-full",
        "w-full",
        "invisible"->Cn.ifTrue(!hasScrolledToInitialIdx),
        switch (direction) {
        | Horizontal => cn(["overflow-x-scroll", "flex", "overflow-y-hidden"])
        | Vertical => cn(["overflow-y-scroll", "overflow-x-hidden"])
        },
      ])}
      style={
        style()
        ->unsafeStyle([
            (
              "scrollSnapType",
              switch (direction) {
              | Horizontal => "x mandatory"
              | Vertical => "y mandatory"
              },
            ),
            ("scrollSnapStop", "always"),
            ("-webkit-overflow-scrolling", "touch"),
          ])
      }>
      {React.array(children)}
    </div>;
  };
};

module Item = {
  [@react.component]
  let make = (~className=?, ~direction, ~children=React.null) => {
    <div
      style={
        style()
        ->unsafeStyle([
            ("scrollSnapStop", "always"),
            ("scrollSnapAlign", "start"),
          ])
      }
      className={cn([
        "w-full",
        "h-full",
        switch (direction) {
        | Horizontal => cn(["flex-shrink-0"])
        | Vertical => ""
        },
        className->Cn.unpack,
      ])}>
      children
    </div>;
  };
};
