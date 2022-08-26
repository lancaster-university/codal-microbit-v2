
function readData(csv) {
    let result = {};

    const rows = csv.split("\n");
    const headings = rows[0].split(",");

    for (let row = 0; row < rows.length; row++) {
        let cells = rows[row].split(",");

        if (row === 0) { // this is the first row, containing our headings
            headings.forEach(function (heading) {
                result[heading] = [];
            });

            continue;
        }

        for (let column = 0; column < headings.length; column++) {
            let value = column >= cells.length ? "" : cells[column];

            result[headings[column]].push(value);
        }
    }

    return result;
}

let baseLoad = Object.getOwnPropertyDescriptor(dl, "load").get;

function load() {
    let offlineRoot = document.querySelector("#w");
    offlineRoot.style.display = "none";

    baseLoad();

    let reactRoot = document.createElement("div");
    reactRoot.id = "root";

    document.body.appendChild(reactRoot);

    let reactScript = document.createElement("script");
    reactScript.src = ""; // todo link to react js

    document.head.appendChild(reactScript);

    window.dataLog = readData(window.x); //x = csv data

    reactScript.addEventListener("load", function () {
        document.body.removeChild(offlineRoot);
    });

    reactScript.addEventListener("error", function () {
        document.body.removeChild(reactRoot);
        offlineRoot.style.display = "";
    });
}

Object.defineProperty(dl, "load", {
    get: load
});