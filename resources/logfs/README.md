# LogFS HTML Resources

This folder contains the HTML+JS used by MicroBitLogFS.

## Requirements

- The overall HTML+JS MUST be byte aligned to 2048 bytes.
- Use trailing spaces to ensure the file to 2048 bytes.
- The file must end with the delimiting characters `<!--FS_START`

## Installation and Updating

This source code is not automatically compiled into the resulting CODAL binary.

There's some NodeJS-based tooling to minimize and munge the file into a C++ array format.

Run `npm install` in this directory to install the dependencies.

To generate a `test.html` file with the sample trailer appended run `npm run test`.

To update the C++ array contents in `source/MicroBitLog.cpp` run `npm run build`.

The build process will exit with an error if it cannot fit the minimised HTML in the 2048 limit.
