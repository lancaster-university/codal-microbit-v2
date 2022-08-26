import Plot from "react-plotly.js";
import { Data } from 'plotly.js';
import { DataLog, VisualisationType } from "./App";

const timestampRegex = /Time \(.+\)/;

const LineGraphVisualisation: VisualisationType = {
    name: "Line Graph",
    availablityError: log => {
        const headings = Object.keys(log);

        if (headings.length < 2) {
            return "Requires two or more columns. Timestamps must be enabled.";
        }

        if (!timestampRegex.test(headings[0])) {
            return "Timestamps must be enabled when logging data.";
        }

        return null;
    },
    generate: log => {
        const headings = Object.keys(log);

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

        splitLogs.push(currentLog);

        const colors = [
            // micro:bit brand colors
            "rgb(0, 200, 0)",
            "rgb(62, 182, 253)",
            "rgb(205, 3, 101)",
            "rgb(231, 100, 92)",
            "rgb(108, 75, 193)",
            "rgb(123, 205, 194)",
        ];

        return (<div>
            {splitLogs.map(log => {
                const data: Data[] = headings.slice(1).map((y, index): Data => {
                    return {
                        name: headings[index + 1],
                        type: "scatter",
                        mode: "lines+markers",
                        x: graphX,
                        y: log[y],
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

                return <Plot
                    data={data}

                    layout={{ width: 800, height: 600, title: "Test", xaxis: { title: headings[0] } }}
                    config={{ displaylogo: false, responsive: true, toImageButtonOptions: { filename: "MY_DATA" }, modeBarButtonsToRemove: ["select2d", "lasso2d", "autoScale2d"] }}
                />;
            })}
        </div>);
    }
};

export default LineGraphVisualisation;