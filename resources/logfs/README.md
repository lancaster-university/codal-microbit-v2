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

To generate test HTML files using sample trailers run `npm run test`.

An equivalently named HTML file is created for each `sample-*.txt` file in this directory.

To update the C++ array contents in `source/MicroBitLog.cpp` run `npm run build`.

The build process will exit with an error if it cannot fit the minimised HTML in the 2048 limit.

## Test files

Each `sample-*.txt` file contains test data to represent the raw flash data in
the data logging storage area, excluding the minified HTML.

The data logging storage is composed like this:

```
  ┌────────────────────────┐ 0x00000
  │          HTML          │
  │ (minified header.html) │
  │------------------------│ 2 KBs
  │                        │
  │        Raw Data        │
  │                        │
  │ (Test data stored in   │
  │   the samples-*.txt    │
  │         files)         │
  │                        │
  └────────────────────────┘ End of storage
```

For each sample file, `npm run test` script minifies the `header.html` and
attaches the raw data to the end of the HTML file.

Some of the `sample-*.txt` files have an equivalent `sample-*.js` file with the
source code of the MakeCode programme used to generate them.

### Test descriptions

- **sample-trailer**: Simple 3 columns table with 11 rows
    - The test programme does a full erase before logging data, so the rest of
      the storage data is blank, with no residual left overs from
      previous data logging runs.
- **sample-trailer-dirty**: The same as the sample-trailer, but the DAPLink
  storage was first filled, and then soft-marked as erased. So the old data is
  still present on the raw blob, but marked as erased.
    - This HTML should render the same data table as `sample-trailer`.
- **sample-trailer-noheader**: The same as the sample-trailer, but manually
  modified to remove the table header.
    - This HTML should render the same data table as `sample-trailer`, but
      without the table header (`Time (seconds)`, `X`, `Y`, `Z`).
    - At the time of writing the header.html styles the first table row with
      bold text, so the first data row entry will look like a table header.
    - Technically, using the CODAL API (and therefore MakeCode or MicroPython)
      shouldn't allow this to happen, as each log entry needs to be paired to a
      column, so if this test fails in the future we might consider removing it.
- **sample-single-column-noheader**: A single column table without a header
  and with 33 data rows.
    - The MakeCode programme to generate this test data leaves the `column`
      fields empty, so the table header contains no entries, and the data table
      in the raw data blob starts with a LF (`\n`).
    - At the time of writing the header.html styles the first row, which is
      normally the table header row, with bold text.
      In this case, as header is empty, the generated HTML starts with an empty
      `<tr></tr>`, which is not visible in Chrome and therefore it looks like
      the HTML renders a table without a header.
- **sample-time-series**: A valid time series table with 4 columns (Time, x,
  y , z) and 172 data rows with increasing time stamps.
    - The raw data contains old logging data marked as erased, that should not
      be rendered in the HTML table.
- **sample-invalid-time-series.txt**: An invalid time series table with 4
  columns (Goat, x, y , z) and 172 data rows with increasing time stamps.
    - For a time series the first column should be named `Time...`, this test
      file was based on `sample-time-series` and the `Time (seconds)` column
      name has been renamed to `Goat (seconds)`.
    - In this test file the "Visual Preview" should not work.
- **sample-time-series-nonsequential**: Based on the `sample-time-series` file,
  a single entry has been modified to make the time stamps non-sequential.
    - Data entry 169 (6th from the bottom) has been changed from `18.99` to
      `99.99`. As the next entry time stamp is `19.1`, this represents a second
      set of logged data after a micro:bit reset.
    - In this test file the "Visual Preview" should plot a graph until entry
      169, and display some kind of warning/error message to indicate it is
      not plotting the entire dataset.
- **sample-full-log**: A full log with 6064 rows.
    - There should be a visible indicator in the HTML page that the log is full.

### Generating new test files

These steps can be followed to create a new sample-*.txt file:
- Create a data logging micro:bit programme
    - This can be in either CODAL C++, MakeCode, or MicroPython
- Copy the MY_DATA.HTML file from the MICROBIT drive into this folder
    - It's important to copy the file rather than save the HTML output from
      a browser, as we need the raw data to be intact
- Rename the file with the `sample-*.txt` format
- With a binary file editor, erase the first 2 KBs of the file
    - The 2048 byte block should end in `<!--FS_START` and the following block
      should start with `UBIT_LOG_FS_V_`
    - The currently reserved storage space for the HTML is 2 KB. If this figure
      changes in the future this step will need to be update to reflect the new
      size
