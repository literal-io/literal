[@decco]
type routeParams = {id: string};

[@react.component]
let default = () => {
  let router = Next.Router.useRouter();

  let highlightId =
    switch (routeParams_decode(router.Next.query)) {
    | Ok(p) => Some(p.id)
    | _ => None
    };

  <QueryRenderers_NewNote highlightId />;
};
