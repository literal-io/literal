[@bs.deriving accessors]
type action = {
  label: string,
  icon: string,
  routes: array(string),
  href: string,
  _as: string,
};

let makeActions = identityId => [|
  {
    label: "Annotations",
    icon: Svg.highlight,
    routes: [|Routes.CreatorsIdAnnotationCollectionsId.staticPath|],
    href:
      Routes.CreatorsIdAnnotationCollectionsId.path(
        ~identityId,
        ~annotationCollectionIdComponent=Lib_GraphQL.AnnotationCollection.recentAnnotationCollectionIdComponent,
      ),
    _as: Routes.CreatorsIdAnnotationCollectionsId.staticPath,
  },
  {
    label: "Settings",
    icon: Svg.manageAccounts,
    routes: [|Routes.CreatorsIdSettings.staticPath|],
    href: Routes.CreatorsIdSettings.path(~identityId),
    _as: Routes.CreatorsIdSettings.staticPath,
  },
|];

external anyComponent: 'a => React.element = "%identity";

[@decco]
type identityParams = {identityId: string};

[@react.component]
let make = () => {
  let router = Next.Router.useRouter();
  let Providers_BottomNavigation.{isVisible: isBottomNavigationVisible} =
    React.useContext(Providers_BottomNavigation.context);
  let identityId =
    switch (identityParams_decode(router.Next.query)) {
    | Ok({identityId}) => Some(identityId)
    | _ => None
    };
  let actions =
    identityId->Belt.Option.map(identityId => makeActions(identityId));
  let value =
    actions
    ->Belt.Option.flatMap(actions =>
        actions->Belt.Array.getBy(({routes}) =>
          routes->Belt.Array.some(route => route == router.route)
        )
      )
    ->Belt.Option.map(action => MaterialUi_Types.Any(action->label));

  let visible =
    Js.Option.isSome(value)
    && Js.Option.isSome(identityId)
    && isBottomNavigationVisible;

  let handleActionClick = ({_as, href}) => {
    Next.Router.pushWithAs(_as, href);
    ();
  };

  <MaterialUi.Slide _in=visible direction=`Up>
    <MaterialUi.BottomNavigation
      ?value
      showLabels=true
      classes={MaterialUi.BottomNavigation.Classes.make(
        ~root=
          Cn.fromList([
            "absolute",
            "bottom-0",
            "left-0",
            "right-0",
            "bg-darkAccent",
            "border-t",
            "border-dotted",
            "border-lightDisabled",
          ]),
        (),
      )}>
      {actions
       ->Belt.Option.getWithDefault([||])
       ->Belt.Array.map(({label, icon} as action) =>
           <MaterialUi.BottomNavigationAction
             label={React.string(label)}
             onClick={_ => handleActionClick(action)}
             icon={<Svg icon />}
             value={MaterialUi_Types.Any(label)}
             classes={MaterialUi.BottomNavigationAction.Classes.make(
               ~root=Cn.fromList(["text-lightSecondary", "opacity-50"]),
               ~label=Cn.fromList(["font-sans"]),
               ~selected=Cn.fromList(["text-lightPrimary", "opacity-100"]),
               (),
             )}
           />
         )
       ->React.array}
    </MaterialUi.BottomNavigation>
  </MaterialUi.Slide>;
};
