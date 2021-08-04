import { readdirSync, readFileSync, writeFileSync, mkdirSync } from "fs";

const OUTPUT_DIR = new URL("../app/src/main/assets/", import.meta.url);
const INPUT_DIR = new URL("./dist/", import.meta.url);

const wrapExecutor = (contents) => contents + "\n Literal.default();";

mkdirSync(OUTPUT_DIR, { recursive: true })

readdirSync(INPUT_DIR).forEach((fileName) => {
  writeFileSync(
    new URL(fileName, OUTPUT_DIR),
    wrapExecutor(readFileSync(new URL(fileName, INPUT_DIR)))
  );
});
