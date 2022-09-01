import Plot from "react-plotly.js";
import { Data } from 'plotly.js';
import { timestampRegex, visualisationConfig, VisualisationType } from "./App";
import "./LineGraphVisualisation.css";
import DataLog from "./DataLog";

const LineGraphVisualisation: VisualisationType = {
    name: "Line Graph",
    availablityError: log => {
        if (log.headers.length < 2) {
            return "Requires two or more columns. Timestamps must be enabled.";
        }

        if (!timestampRegex.test(log.headers[0])) {
            return "Timestamps must be enabled when logging data.";
        }

        return null;
    },
    generate: log => {

        const splitLogs = log.split((row, prevRow) => !row.isHeading && !!prevRow && Number(row.data[0]) < Number(prevRow.data[0]));


        /*const headings = Object.keys(log);

        // todo: error when going back in time!

        //if (abortRow) {
        //  rows[abortRow].id = "abort-row";
        //}

        const graphX = log[headings[0]];
        const graphY = log;

        const splitLogs: DataLog[] = [];

        let prevTimestamp = 0;

        let currentLog: DataLog = {};

        headings.forEach(heading => currentLog[heading] = []);

        for (let y = 0; y < graphX.length; y++) {
            if (Number(graphX[y]) < prevTimestamp) {
                // we've gone back in time! split the log here to create multiple graphs...

                splitLogs.push(currentLog);

                currentLog = {};

                headings.forEach(heading => currentLog[heading] = []);
            }

            headings.forEach(heading => {
                currentLog[heading].push(log[heading][y]);
            });

            prevTimestamp = Number(graphX[y]);
        }

        splitLogs.push(currentLog);*/

        const colors = [
            // micro:bit brand colors
            "rgb(0, 200, 0)",
            "rgb(62, 182, 253)",
            "rgb(205, 3, 101)",
            "rgb(231, 100, 92)",
            "rgb(108, 75, 193)",
            "rgb(123, 205, 194)",
        ];

        let currentRow = 1;

        const graphX = log.dataForHeader(0);

        return (<div>
            {splitLogs.map(log => {
                const data: Data[] = log.headers.slice(1).map((header, index): Data => {
                    return {
                        name: header,
                        type: "scatter",
                        mode: "lines+markers",
                        x: graphX,
                        y: log.dataForHeader(header),
                        line: {
                            color: colors[(index - 1) % colors.length],
                        },
                        marker: {
                            // There are more than this but they look increasingly odd.
                            symbol: (index - 1) % 24,
                        },
                    };
                }
                );

                let rowFrom = currentRow;
                let rowTo = (currentRow += log.data.length) - 1;

                return [
                <div className="graph-span">Rows {rowFrom} - {rowTo}</div>,
                <Plot
                    data={data}

                    className="graph"

                    layout={{ height: 500, margin: {l: 60, r: 60, t: 30, b: 70}, xaxis: { title: log.headers[0] } }}
                    config={visualisationConfig}
                />
            ];
            })}
        </div>);
    }
};

export default LineGraphVisualisation;