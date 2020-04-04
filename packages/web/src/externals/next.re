type router = {query: Js.Json.t};

[@bs.module "next/router"] external useRouter: unit => router = "useRouter";
