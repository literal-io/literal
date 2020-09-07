#!/usr/bin/env node

const fs = require("fs");
const path = require("path");

const schema = require("../graphql-schema.json");

const fragmentTypes = schema.data.__schema.types.filter(
  (type) => type.possibleTypes !== null
);

schema.data.__schema.types = fragmentTypes;

fs.writeFileSync(
  path.resolve(__dirname, "../fragment-types.json"),
  JSON.stringify(schema.data)
);
