export interface DataLogRow {
    isHeading?: boolean;
    data: string[];
}

export default class DataLog {

    constructor(public headers: string[], public data: DataLogRow[]) {
    }

    public static fromCSV(csv: string): DataLog {
        const rows = csv.replace("\r", "").split("\n");
        const headers = rows[0].split(",");
        const data: DataLogRow[] = [];

        rows.forEach((row, index) => {
            const cols = row.split(",");

            data.push({data: cols, isHeading: index === 0});
        });

        return new DataLog(headers, data);
    }

    public dataForHeader(header: string | number): string[] {
        const index = typeof header === "number" ? header : this.headers.indexOf(header);

        if (index === -1) {
            return [];
        }

        return this.data.filter(data => !data.isHeading).map(data => data.data[index]);
    }

    public toCSV(): string {
        return this.data.map(row =>
            row.data
                .map(value => value.replaceAll('"', '""'))
                .map(value => `"${value}"`)
                .join(",")
        ).join("\n");
    }

    public split(where: (row: DataLogRow, previousRow: DataLogRow | null, rowIndex: number) => boolean) {

        const splitLogs: DataLog[] = [];

        let prevRow: DataLogRow | null = null;

        let currentData: DataLogRow[] = [];

        for (let i = 0; i < this.data.length; i++) {
            const row = this.data[i];

            if (where(row, prevRow, i)) { // if we should split here...
                
                splitLogs.push(new DataLog(this.headers, currentData));
                currentData = [];
            }

            currentData.push(row);

            prevRow = row;
        }

        splitLogs.push(new DataLog(this.headers, currentData));

        return splitLogs;
    }
}