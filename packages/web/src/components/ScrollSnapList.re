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
          directedScrollTargetIdx.current = Some(idx);
          let width = Webapi.Dom.Element.clientWidth(elem);
          let _ =
            Raw.maybeScrollTo(
              elem,
              {
                "behavior": behavior,
                "left": float_of_int(width * idx),
                "top": 0.,
              },
            );
          let _ = setHasScrolledToInitialIdx(_ => true);
          ();
        | (Some(elem), Vertical) =>
          directedScrollTargetIdx.current = Some(idx);
          let height = Webapi.Dom.Element.clientHeight(elem);
          let _ =
            Raw.maybeScrollTo(
              elem,
              {
                "behavior": behavior,
                "left": 0.,
                "top": float_of_int(height * idx),
              },
            );
          let _ = setHasScrolledToInitialIdx(_ => true);
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
        containerRef.current
        ->Js.Nullable.toOption
        ->Belt.Option.forEach(container =>
            if (Raw.unsafeEq(ReactEvent.UI.target(ev), container)) {
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

              if (scroll != prevScroll.current->Belt.Option.getWithDefault(-1)) {
                prevScroll.current = Some(scroll);
                let relativePos = scroll /. float_of_int(containerDimen);
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
            }
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
