/**
 * Script to create minfied HTML from the source HTML file.
 *
 * Run it via the NPM scripts. See docs in README.md.
 */
const minify = require("html-minifier").minify;
const fs = require("fs");

const input = fs.readFileSync("header.html", { encoding: "ascii" });
const delimiter = "<!--FS_START";
let result = minify(input, {
  collapseBooleanAttributes: true,
  collapseInlineTagWhitespace: true,
  collapseWhitespace: true,
  minifyCSS: true,
  minifyJS: true,
  minifyURLs: true,
  removeAttributeQuotes: true,
  removeComments: true,
  removeOptionalTags: true,
  removeRedundantAttributes: true,
  removeScriptTypeAttributes: true,
  removeStyleLinkTypeAttributes: true,
  useShortDoctype: true,
  removeAttributeQuotes: true,
});

// The overall HTML+JS MUST be byte aligned to 2048 bytes.
// Use trailing spaces to ensure the file to 2048 bytes.
// The file must end with the delimiting characters <!--FS_START
const limit = 2048;
const maxSize = limit - delimiter.length;
const padding = maxSize - result.length;
if (padding < 0) {
  console.error(
    `Could not minimize to ${maxSize} bytes. Difference: ${Math.abs(padding)}`
  );
  process.exit(1);
}
console.log(`${padding} bytes remaining`);
result += " ".repeat(padding);
result += delimiter;

if (result.length !== limit) {
  throw new Error(`Expected ${limit} but was ${result.length}`);
}

if (process.argv[2] === "test") {
  const trailers = fs
    .readdirSync(".")
    .filter((f) => /^sample-.+\.txt$/.test(f));
  // Enable local testing of included CSS/JS. Do it after byte count.
  result = result.replace(/https:\/\/microbit.org\/dl\/\d\//g, "./");
  for (const trailerName of trailers) {
    const trailer = fs.readFileSync(trailerName);
    const htmlName = trailerName.replace(/\.txt$/, ".html");
    fs.writeFileSync(
      htmlName,
      Buffer.concat([Buffer.from(result, { encoding: "utf-8" }), trailer])
    );
  }
} else {
  const cppFile = "../../source/MicroBitLog.cpp";
  const cppContents = fs.readFileSync(cppFile, {
    encoding: "utf-8",
  });
  const arrayContents = Array.from(result)
    .map((c) => "0x" + c.charCodeAt(0).toString(16))
    .join(",");
  const headerRegExp = /MicroBitLog::header\[2048\] = \{[^}]*\}/;
  if (!headerRegExp.test(cppContents)) {
    throw new Error(`Could not find header. Review changes to ${cppFile}.`);
  }
  const replaced = cppContents.replace(
    headerRegExp,
    "MicroBitLog::header[2048] = {" + arrayContents + "}"
  );
  fs.writeFileSync(cppFile, replaced, { encoding: "utf-8" });
}
