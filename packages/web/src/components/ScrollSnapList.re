open Styles;

type direction =
  | Horizontal
  | Vertical;

module Container = {
  [@react.component]
  let make =
    React.forwardRef(
      (
        ~direction,
        ~onIdxChange: option(int => unit)=?,
        ~initialIdx=?,
        ~className=?,
        ~children,
        ref_,
      ) => {
      let containerRef = React.useRef(Js.Nullable.null);
      let directedScrollTargetIdx = React.useRef(None);
      let prevScroll = React.useRef(None);
      let debouncedOnScroll =
        React.useRef(
          Lodash.Throttled3.make(
            (. scrollTop, scrollLeft, onIdxChange) => {
              let (scroll, containerDimen) =
                switch (direction) {
                | Vertical =>
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
                  let windowWidth = Webapi.Dom.(window->Window.innerWidth);
                  (scrollLeft, windowWidth);
                };

              if (scroll != prevScroll.current->Belt.Option.getWithDefault(-1)) {
                prevScroll.current = Some(scroll);
                let relativePos =
                  float_of_int(scroll) /. float_of_int(containerDimen);
                let activeIdx =
                  switch (Constants.browser()->Bowser.getBrowserName) {
                  | Some(`Chrome) => relativePos->Js.Math.round->int_of_float
                  | Some(`Safari)
                  | _ => relativePos->Js.Math.floor
                  };

                switch (directedScrollTargetIdx.current) {
                | Some(idx) when activeIdx == idx =>
                  directedScrollTargetIdx.current = None
                | Some(_) => ()
                | None =>
                  onIdxChange->Belt.Option.forEach(fn => fn(activeIdx))
                };
              };
            },
            200,
          ),
        );

      let (hasScrolledToInitialIdx, setHasScrolledToInitialIdx) =
        React.useState(() =>
          switch (initialIdx) {
          | Some(idx) when idx === 0 => true
          | Some(_)
          | None => false
          }
        );

      let scrollToIdx = (~behavior="auto", idx) => {
        let elem = containerRef->React.Ref.current->Js.Nullable.toOption;
        switch (elem, direction) {
        | (Some(elem), Horizontal) =>
          let currentScrollLeft = Webapi.Dom.Element.scrollLeft(elem);
          let targetScrollLeft =
            float_of_int(Webapi.Dom.Element.clientWidth(elem) * idx);
          if (targetScrollLeft != currentScrollLeft) {
            directedScrollTargetIdx.current = Some(idx);
            let _ =
              Raw.maybeScrollTo(
                elem,
                {"behavior": behavior, "left": targetScrollLeft, "top": 0.},
              );
            let _ = setHasScrolledToInitialIdx(_ => true);
            ();
          };
        | (Some(elem), Vertical) =>
          let currentScrollTop = Webapi.Dom.Element.scrollTop(elem);
          let targetScrollTop =
            float_of_int(Webapi.Dom.Element.clientHeight(elem) * idx);
          if (targetScrollTop != currentScrollTop) {
            directedScrollTargetIdx.current = Some(idx);
            let _ =
              Raw.maybeScrollTo(
                elem,
                {"behavior": behavior, "left": 0., "top": targetScrollTop},
              );
            let _ = setHasScrolledToInitialIdx(_ => true);
            ();
          };
          ();
        | _ => ()
        };
      };

      let _ =
        React.useImperativeHandle0(ref_, () => {"scrollToIdx": scrollToIdx});

      let _ =
        React.useEffect0(() => {
          let _ =
            switch (initialIdx) {
            | Some(initialIdx) when initialIdx > 0 && !hasScrolledToInitialIdx =>
              scrollToIdx(initialIdx)
            | _ => ()
            };
          None;
        });

      let onScroll = ev => {
        debouncedOnScroll.current(.
          ReactEvent.UI.target(ev)##scrollTop,
          ReactEvent.UI.target(ev)##scrollLeft,
          onIdxChange,
        );
      };

      <div
        onScroll=?{
          hasScrolledToInitialIdx && Js.Array2.length(children) > 0
            ? Some(onScroll) : None
        }
        ref={containerRef->ReactDOMRe.Ref.domRef}
        className={Cn.fromList([
          className->Belt.Option.getWithDefault(
            Cn.fromList(["h-full", "w-full"]),
          ),
          "invisible"->Cn.ifTrue(!hasScrolledToInitialIdx),
          switch (direction) {
          | Horizontal =>
            cn(["overflow-x-scroll", "flex", "overflow-y-hidden"])
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
    });
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
      className={Cn.fromList([
        className->Belt.Option.getWithDefault(
          Cn.fromList(["w-full", "h-full"]),
        ),
        switch (direction) {
        | Horizontal => cn(["flex-shrink-0"])
        | Vertical => ""
        },
      ])}>
      children
    </div>;
  };
};
