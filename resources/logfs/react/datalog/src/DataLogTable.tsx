import React from "react";
import { DataLog } from "./App";
import "./DataLogTable.css";

type DataLogProps = {
    log: DataLog
}

function DataLogTable(props: DataLogProps) {
    // get the column with the highest row count
    const headers = Object.keys(props.log);
    const logLength = headers.map(header => props.log[header].length).reduce((p, c) => p > c ? p : c); // p = our highest row count

    const rows = [];

    for (let i = 0; i < logLength; i++) {
        const row: JSX.Element[] = [];

        headers.forEach((header, index) => row.push(<td key={index}>{props.log[header][i] ?? 'None'}</td>));

        rows.push(<tr key={i}>{row}</tr>);
    }

    return (
        <table>
            <tbody>
                <tr>
                    {headers.map((header, index) => <td key={index}>{header}</td>)}
                </tr>
                {rows}
                <tr>
                    <td colSpan={headers.length} id="datalog-end">End of data log</td>
                </tr>
            </tbody>
        </table>
    );
}

export default DataLogTable;