/**
 * Data logging support file for "online" mode.
 * This content is managed in https://github.com/lancaster-university/codal-microbit-v2/
 *
 * This code should compile on IE11 and run sufficiently to show the table and the IE11
 * support notice.
 */

function UserGraphError(message) {
  let instance = new Error(message);
  Object.setPrototypeOf(instance, Object.getPrototypeOf(this));
  if (Error.captureStackTrace) {
    Error.captureStackTrace(instance, UserGraphError);
  }
  return instance;
}

(function () {
  let isParentPage = !location.href.split("?")[1];
  if (isParentPage) {
    const base = Object.keys(window.dl).reduce(function (acc, curr) {
      acc[curr] = window.dl[curr];
      return acc;
    }, {});
    // Overrides that provide additional behaviour when online.
    const overrides = {
      update: alert.bind(
        null,
        "To see the latest data that changed after you opened this file, you must unplug your micro:bit and plug it back in."
      ),
      clear: alert.bind(
        null,
        "The log is cleared when you reflash your micro:bit. Your program can include code or blocks to clear the log when you choose."
      ),
      copy: function () {
        base.copy();
        let button = window.event.currentTarget;
        button.innerText = "Copied";
        setTimeout(function () {
          button.innerText = "Copy";
        }, 1000);
      },
      load: function () {
        let wrapper = document.querySelector("#w");
        let isIE = Boolean(document.documentMode);
        if (isIE) {
          const browserWarning = wrapper.appendChild(
            document.createElement("p")
          );
          browserWarning.id = "browser-warning";
          browserWarning.innerText =
            "Internet Explorer (IE) is not supported. Please use Google Chrome, Microsoft Edge, Mozilla Firefox or Safari.";
        }

        let header = document.body.insertBefore(
          document.createElement("header"),
          wrapper
        );
        header.outerHTML =
          '<header><div class="header-strip"></div><div class="header-contents"><a href="https://microbit.org"><svg role="img" aria-labelledby="microbit-logo" viewBox="0 0 166.8 28.7" width="167" height="40"><title id="microbit-logo">micro:bit logo</title><path fill="#fff" d="M71.7 23.1h-3.3V14c0-2-1.2-3.5-2.8-3.5-1.6 0-2.8 1.4-2.8 3.5v9.1h-3.3V14c0-2.3-1.4-3.5-2.8-3.5-1.6 0-2.8 1.4-2.8 3.5v9.1h-3.3v-9c0-4.1 2.5-6.9 6.1-6.9 1.7 0 3.1.7 4.4 2.1 1.2-1.4 2.7-2.1 4.5-2.1 3.6 0 6 2.9 6 6.9v9zM77.7 23.1h-3.3V7.6h3.3v15.5zM76 5.3c-1.2 0-2.1-.9-2.1-2.1 0-1.2.9-2.1 2.1-2.1 1.2 0 2.1.9 2.1 2.1.1 1.2-.9 2.1-2.1 2.1zM88.1 23.5c-2.1 0-4.2-.9-5.7-2.4s-2.3-3.5-2.3-5.8c0-2.2.8-4.3 2.3-5.8C84 8.1 86 7.2 88.1 7.2c2.3 0 4.3.8 5.8 2.3l.4.4-2.3 2.4-.4-.4c-1.1-1-2.2-1.5-3.4-1.5-2.6 0-4.8 2.2-4.8 4.9s2.1 4.9 4.8 4.9c1.2 0 2.3-.5 3.4-1.4l.4-.4 2.4 2.3-.5.4c-1.8 1.6-3.7 2.4-5.8 2.4zM100.1 23.1h-3.5v-7.8c0-5 2.2-7.4 7-7.8l.7-.1v3.3l-.5.1c-2.6.3-3.6 1.5-3.6 4.3v8zM113.4 23.5c-2.1 0-4.2-.9-5.7-2.4s-2.3-3.6-2.3-5.8c0-2.2.8-4.2 2.3-5.8 1.5-1.6 3.5-2.4 5.7-2.4 2.1 0 4.1.9 5.6 2.4s2.3 3.6 2.3 5.8c0 2.2-.8 4.3-2.3 5.8-1.5 1.6-3.5 2.4-5.6 2.4zm-.1-13c-2.5 0-4.6 2.2-4.6 4.9s2.1 4.9 4.6 4.9c2.6 0 4.6-2.1 4.6-4.9.1-2.8-2-4.9-4.6-4.9zM126.8 23.1c-1.2 0-2.3-1-2.3-2.3 0-1.3 1-2.3 2.3-2.3 1.3 0 2.3 1 2.3 2.3-.1 1.3-1.1 2.3-2.3 2.3zM126.8 11.7c-1.2 0-2.3-1-2.3-2.3 0-1.3 1-2.3 2.3-2.3 1.3 0 2.3 1 2.3 2.3-.1 1.3-1.1 2.3-2.3 2.3zM140.2 23.5c-4.7 0-8-3.7-8-8.9V.2h3.3v8.5c1.4-1 3-1.5 4.7-1.5 2.1 0 4.1.8 5.6 2.4 1.5 1.5 2.3 3.6 2.3 5.8 0 2.2-.8 4.3-2.3 5.8-1.5 1.5-3.5 2.3-5.6 2.3zm0-13c-2.6 0-4.8 2.2-4.8 4.9s2.1 4.9 4.8 4.9c2.6 0 4.8-2.2 4.8-4.9-.1-2.7-2.2-4.9-4.8-4.9zM154.3 23.1H151V7.6h3.3v15.5zm-1.7-17.8c-1.2 0-2.1-.9-2.1-2.1 0-1.2.9-2.1 2.1-2.1 1.2 0 2.1.9 2.1 2.1.1 1.2-.9 2.1-2.1 2.1zM166.6 23.6l-.7-.1c-4.9-.9-6.7-3.3-6.7-8.6V11h-1.4V7.8h1.4V4.3h3.3v3.4h4.1v3.2h-4.1v4.4c0 2.9 1.1 4.4 3.6 4.7l.5.1v3.5z"></path><g fill="#fff"><path d="M32.5 18c1.5 0 2.6-1.2 2.6-2.6s-1.2-2.6-2.6-2.6c-1.5 0-2.6 1.2-2.6 2.6s1.2 2.6 2.6 2.6M13.3 12.8c-1.5 0-2.6 1.2-2.6 2.6s1.2 2.6 2.6 2.6c1.4 0 2.6-1.2 2.6-2.6s-1.2-2.6-2.6-2.6"></path><path d="M13.3 7.6c-4.3 0-7.8 3.5-7.8 7.8s3.5 7.8 7.8 7.8h19.5c4.3 0 7.8-3.5 7.8-7.8s-3.5-7.8-7.8-7.8H13.3m19.5 20.8H13.3c-7.2 0-13-5.8-13-13s5.8-13 13-13h19.5c7.2 0 13 5.8 13 13s-5.9 13-13 13"></path></g></svg></a></div></header>';

        let info = document.createElement("p");
        info.id = "info";
        wrapper.appendChild(info);

        let buttonBar = document.querySelector(".bb");
        let graphButton = buttonBar.appendChild(
          document.createElement("button")
        );
        graphButton.id = "graphButton";

        window.dl.resetGraph();

        if (window.Promise) {
          graphButton.onclick = window.dl.graph;
          let graphLib = document.createElement("script");
          window.dl.graphLibPromise = new Promise(function (resolve, reject) {
            graphLib.onload = resolve;
            graphLib.onerror = reject;
          });
          graphLib.src =
            "https://microbit.org/dl/libs/plotly/plotly-scatter-6291fec6d1543144ba90.js";
          document.head.appendChild(graphLib);
        }

        // base.load() returns the raw data contained after FS_START, meaning we
        // can parse it again here to fetch the DAPLink version - saves bytes!
        const rawData = base.load();

        this.daplinkVersion = parseInt(rawData.substring(40, 44));
      },

      hasGraph: function () {
        return Boolean(document.querySelector("#graph"));
      },

      resetGraph: function () {
        document.querySelector("#info").innerHTML =
          "This is the data on your micro:bit. To analyse it and create your own graphs, transfer it to your computer. You can copy and paste your data, or download it as a CSV file which you can import into a spreadsheet or graphing tool. <a href='https://microbit.org/get-started/user-guide/data-logging/'>Learn more about micro:bit data logging</a>.";
        let graphButton = document.querySelector("#graphButton");
        graphButton.innerText = "Visual preview";

        let abortRow = document.querySelector("#abort-row");
        if (abortRow) {
          abortRow.id = null;
        }

        let graph = document.querySelector("#graph");
        if (graph) {
          graph.remove();
        }
      },

      graph: function () {
        function readData(table) {
          if (table.rows.length === 0) {
            throw new UserGraphError("No data to graph.");
          }
          const rows = table.rows;
          const headings = [].slice.call(table.rows[0].cells).map(function (r) {
            return r.innerText;
          });
          if (headings.length < 2) {
            throw new UserGraphError(
              "The visual preview requires two or more columns. Timestamps must be enabled."
            );
          }
          if (!/Time \(.+\)/.test(headings[0])) {
            throw new UserGraphError(
              "To show a visual preview, timestamps must be enabled when logging data."
            );
          }
          let x = [];
          let ys = headings.slice(1).map(function () {
            return [];
          });
          let abortRow;
          outer: for (let r = 1; r < rows.length; ++r) {
            let cells = rows[r].cells;
            if (cells.length === 0) {
              // Last row expected to be empty. Might be able to fix in offline code.
              continue;
            }
            if (cells.length !== headings.length) {
              // Stop processing if the data changes shape.
              abortRow = r;
              break;
            }
            for (let c = 0; c < cells.length; ++c) {
              let v = Number(cells[c].innerText);
              if (isNaN(v)) {
                abortRow = r;
                break outer;
              }
              if (c === 0) {
                if (x[x.length - 1] > v) {
                  // Time cannot go backwards.
                  abortRow = r;
                  break outer;
                }
                x.push(v);
              } else {
                ys[c - 1].push(v);
              }
            }
          }
          if (abortRow) {
            rows[abortRow].id = "abort-row";
          }

          const colors = [
            // micro:bit brand colors
            "rgb(0, 200, 0)",
            "rgb(62, 182, 253)",
            "rgb(205, 3, 101)",
            "rgb(231, 100, 92)",
            "rgb(108, 75, 193)",
            "rgb(123, 205, 194)",
          ];
          let nextColor = 0;
          let nextMarker = 0;
          return {
            xAxisLabel: headings[0],
            abortRow: abortRow,
            data: ys.map(function (y, index) {
              return {
                name: headings[index + 1],
                type: "scatter",
                mode: "lines+markers",
                x: x,
                y: y,
                line: {
                  color: colors[nextColor++ % colors.length],
                },
                marker: {
                  // There are more than this but they look increasingly odd.
                  symbol: nextMarker++ % 24,
                },
              };
            }),
          };
        }

        function toggleGraph() {
          if (window.dl.hasGraph()) {
            window.dl.resetGraph();
            return;
          }

          let graphHeight = Math.max(
            450,
            Math.round(window.innerHeight * 0.75)
          );
          let graphWrapper = document.createElement("div");
          graphWrapper.id = "graph";
          graphWrapper.style.height = graphHeight;
          graphWrapper.style.textAlign = "center";
          window.requestAnimationFrame(function () {
            let loading = graphWrapper.appendChild(document.createElement("p"));
            loading.style.marginTop = "20vh";
            loading.innerHTML = "Loading visual preview&mldr;";
            let info = document.querySelector("#info");
            info.insertAdjacentElement("afterend", graphWrapper);

            window.requestAnimationFrame(function () {
              try {
                let table = document.querySelector("table");
                let graphData = readData(table);
                let graphFadeContainer = graphWrapper.appendChild(
                  document.createElement("div")
                );
                if (graphData.abortRow) {
                  let warningElement = graphFadeContainer.appendChild(
                    document.createElement("p")
                  );
                  warningElement.id = "graph-warning";
                  warningElement.innerHTML =
                    "The graph only includes rows down to <a href='#abort-row'>row " +
                    (graphData.abortRow + 1) +
                    "</a>. The visual preview does not support repeated headers or non-numeric data. The first column must be a timestamp.";
                }
                let graphContainer = graphFadeContainer.appendChild(
                  document.createElement("div")
                );
                graphContainer.setAttribute("role", "img");
                graphContainer.setAttribute(
                  "aria-label",
                  "Autogenerated line graph of the data in the table below."
                );
                graphFadeContainer.style.opacity = 0;

                info.style.transition = "none";
                // We fade it in along with the graph.
                info.style.opacity = 0;
                info.innerHTML =
                  "This is a visual preview of the data on your micro:bit. To analyse it in more detail or create your own graphs, transfer it to your computer. You can copy and paste your data, or download it as a CSV file which you can import into a spreadsheet or graphing tool. <a href='https://microbit.org/get-started/user-guide/data-logging/'>Learn more about micro:bit data logging</a>.";

                Plotly.newPlot(
                  graphContainer,
                  graphData.data,
                  {
                    height: graphHeight,
                    xaxis: {
                      title: graphData.xAxisLabel,
                      showgrid: true,
                    },
                    yaxis: {
                      showgrid: true,
                    },
                  },
                  {
                    displaylogo: false,
                    responsive: true,
                    toImageButtonOptions: {
                      filename: "MY_DATA",
                    },
                    modeBarButtonsToRemove: [
                      "select2d",
                      "lasso2d",
                      "autoScale2d",
                    ],
                  }
                );
                graphContainer.on("plotly_afterplot", function () {
                  document.querySelector("#graphButton").innerText =
                    "Close visual preview";

                  // We originally set the height for the loading indicator but must
                  // remove it now as we may use more space for any error message.
                  graphWrapper.style.height = null;
                  loading.remove();

                  let transition = "opacity 600ms linear";
                  graphFadeContainer.style.transition = transition;
                  info.style.transition = transition;
                  graphFadeContainer.style.opacity = 1;
                  info.style.opacity = 1;
                });
              } catch (e) {
                window.dl.resetGraph();

                if (e instanceof UserGraphError) {
                  alert(e.message);
                } else {
                  console.error(e);
                  alert("Unknown error creating the graph.");
                }
              }
            });
          });
        }

        // Not on IE11.
        if (window.dl.graphLibPromise) {
          window.dl.graphLibPromise.then(toggleGraph);
        }
      },
    };
    Object.keys(overrides).forEach(function (k) {
      window.dl[k] = overrides[k];
    });
  }
})();
