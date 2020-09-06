#!/usr/bin/env node

const { readFileSync, mkdirSync, writeFileSync } = require("fs");
const { resolve, dirname } = require("path");

if (process.argv.length === 2) {
  return;
}

const path = process.argv[2];

if (path.includes("src/routes/Route_")) {
  const file = readFileSync(path, { encoding: "utf-8" });
  const [_, page] = file.match(/var page \= "(.*)".*$/m) || [];

  if (page) {
    const absPagePath = resolve(__dirname, "../src/pages", page)
    mkdirSync(dirname(absPagePath), { recursive: true })
    const output = `export { default } from "${path}"`
    writeFileSync(absPagePath, output)
  } else {
    throw new Error(
      `Expected \"page\" declaration in Route ${path}, but found none.`
    );
  }
}
