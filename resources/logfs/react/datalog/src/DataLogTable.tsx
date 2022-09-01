import React from "react";
import { timestampRegex } from "./App";
import DataLog from "./DataLog";
import "./DataLogTable.css";

type DataLogProps = {
    log: DataLog,
    highlightDiscontinuousTimes?: boolean
}

function DataLogTable(props: DataLogProps) {
    // get the column with the highest row count
    //const headers = Object.keys(props.log);
    //const logLength = headers.map(header => props.log[header].length).reduce((p, c) => p > c ? p : c); // p = our highest row count

    const headers = props.log.headers;
    const logLength = props.log.data.length;

    const rows = [];

    let prevRowTimestamp = 0;

    for (let i = 0; i < logLength; i++) {
        const row: JSX.Element[] = [];

        const firstRowValue = Number(props.log.dataForHeader(0)[i]);

        const discontinous = (props.highlightDiscontinuousTimes && timestampRegex.test(headers[0]) && firstRowValue < prevRowTimestamp);
        prevRowTimestamp = firstRowValue;

        props.log.data[i].data.forEach((data, index) => row.push(
            <td key={index}>
                {data ?? 'None'}
            </td>
        ));

        rows.push(<tr key={i} className={discontinous ? "discontinous" : ""}>{row}</tr>);
    }

    return (
        <table>
            <tbody>
                {rows}
                <tr>
                    <td colSpan={headers.length} id="datalog-end">End of data log</td>
                </tr>
            </tbody>
        </table>
    );
}

export default DataLogTable;