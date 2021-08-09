let done_: string = [%raw "require('./done.svg')"];
let close: string = [%raw "require('./close.svg')"];
let back: string = [%raw "require('./back.svg')"];
let removeCircle: string = [%raw "require('./remove-circle.svg')"];
let delete: string = [%raw "require('./delete.svg')"];
let logo: string = [%raw "require('./logo.svg')"];
let arrowRight: string = [%raw "require('./arrow-right.svg')"];
let arrowDown: string = [%raw "require('./arrow-down.svg')"];
let arrowUp: string = [%raw "require('./arrow-up.svg')"];
let add: string = [%raw "require('./add.svg')"];
let addCircle: string = [%raw "require('./add-circle.svg')"];
let textFields: string = [%raw "require('./text-fields.svg')"];
let textSnippet: string = [%raw "require('./text-snippet.svg')"];
let label: string = [%raw "require('./label.svg')"];
let helpOutline: string = [%raw "require('./help-outline.svg')"];
let language: string = [%raw "require('./language.svg')"];
let waves: string = [%raw "require('./waves.svg')"];
let android: string = [%raw "require('./android.svg')"];
let apple: string = [%raw "require('./apple.svg')"];
let errorOutline: string = [%raw "require('./error-outline.svg')"];
let more: string = [%raw "require('./more.svg')"];
let article: string = [%raw "require('./article.svg')"];
let manageAccounts: string = [%raw "require('./manage-accounts.svg')"];
let highlight: string = [%raw "require('./highlight.svg')"];
let source: string = [%raw "require('./source.svg')"];
let shareBlack: string = [%raw "require('./share-black.svg')"];
let deleteBlack: string = [%raw "require('./delete-black.svg')"];

[@react.component]
let make = (~className=?, ~style=?, ~onClick=?, ~icon) => {
  <img ?className ?style ?onClick src=icon />;
};
