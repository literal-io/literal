import mjml from "mjml";

import fs from "fs";
import path from "path";

const OUT_DIR = path.resolve(__dirname, "./dist");
const MJML_OPTIONS = {
  fonts: {
    "Roboto Mono":
      "https://fonts.googleapis.com/css2?family=Roboto+Mono&display=swap",
    Domine: "https://fonts.googleapis.com/css2?family=Domine&display=swap",
  },
};

fs.mkdirSync(OUT_DIR, { recursive: true })

fs.readdirSync(path.resolve(__dirname, "./templates")).forEach((fileName) => {
  const filePath = path.resolve(
    __dirname,
    "./templates",
    fileName,
    "template.mjml"
  );
  const fileContent = fs.readFileSync(filePath, { encoding: "utf8" });
  const outPath = path.resolve(OUT_DIR, fileName + ".html");

  const { html, errors } = mjml(fileContent, MJML_OPTIONS);

  if (errors) {
    errors.forEach((e) => {
      throw e;
    });
  }

  fs.writeFileSync(outPath, html);
});
