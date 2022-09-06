import React, { useState } from 'react';
import logo from './logo.svg';
import './App.css';
import Header from './Header';
import DataLogTable from './DataLogTable';
import Plot from 'react-plotly.js';
import { Config, Data } from 'plotly.js';
import LineGraphVisualisation from './LineGraphVisualisation';
import { throws } from 'assert';
import MapVisualisation from './MapVisualisation';
import DropDownButton from './DropDownButton';
import Modal, { ModalContents, ModalProps } from './Modal';
import DataLog from './DataLog';
import { gpsData } from './sample-data';
import { RiClipboardLine, RiDeleteBin2Line, RiDownload2Line, RiRefreshLine, RiShareLine } from "react-icons/ri";
import { IconContext } from 'react-icons';

export interface VisualisationType {
  name: string;
  icon: JSX.Element;
  availablityError: (log: DataLog) => string | null;
  generate: (log: DataLog) => JSX.Element;
}

export const timestampRegex = /Time \(.+\)/;

export const visualisationConfig: Partial<Config> = { displaylogo: false, responsive: true, toImageButtonOptions: { filename: "MY_DATA" }, modeBarButtonsToRemove: ["select2d", "lasso2d", "autoScale2d"] }

const visualisations: VisualisationType[] = [
  LineGraphVisualisation, MapVisualisation
];

interface ShareTarget {
  name: string;
  icon: JSX.Element;
  onShare: (log: DataLog) => any;
}

const shareTargets: ShareTarget[] = [
  {
    name: "Download",
    icon: <RiDownload2Line />,
    onShare(log) {
      const anchor = document.createElement("a");

      anchor.download = "microbit.csv";
      anchor.href = URL.createObjectURL(new Blob([log.toCSV()], { type: "text/csv" }));
      anchor.click();
      anchor.remove();
    }
  },
  {
    name: "Share",
    icon: <RiShareLine />,
    onShare(log) {
      const file = new File([new Blob([log.toCSV()], { type: "text/csv" })], "microbit.csv", { type: "text/csv" });

      navigator.share({
        title: "My micro:bit data",
        files: [file]
      });
    }
  }
];

export function App() {
  const [visualisation, setVisualisation] = useState<VisualisationType | null>(null);
  const [modal, setModal] = useState<ModalProps | null>(null);
  //@ts-ignore
  const [log, setLog] = useState<DataLog>(window.dataLog || gpsData);

  const showModal = (content: ModalContents) => {
    setModal({ ...content, onClose: () => setModal(null) });
  }

  const visualPreviews = visualisations.filter(vis => !vis.availablityError(log));

  const copy = () => {

  }

  const updateData = () => {
    showModal({ title: "Updating Data", content: <div>To see the latest data that changed after you opened this file, you must unplug your micro:bit and plug it back in.</div> });
  }

  const clearLog = () => {
    showModal({ title: "Clearing Log", content: <div>The log is cleared when you reflash your micro:bit. Your program can include code or blocks to clear the log when you choose. <a href="https://microbit.org/get-started/user-guide/data-logging/">Learn more about data logging</a>.</div> });
  }

  const visualise = (visIndex: number) => {
    if (visIndex === -1) { // clicked the main section of the button
      if (visualisation) {
        setVisualisation(null);
        return;
      }

      visIndex = 0; // load the first visualisation
    }


    setVisualisation(visualPreviews[visIndex]);
  }

  const debugCSV = () => {
    const data = prompt("csv data?") || "";

    setLog(DataLog.fromCSV(data));
  }

  const handleShare = (index: number = 0) => {
    shareTargets[index].onShare(log);
  }

  return (
    <IconContext.Provider value={{ className: "icon" }}>
      <div className="app">
        {modal && <Modal {...modal} />}
        <Header />
        <main>
          <h1>micro:bit data log</h1>
          <div className="buttons">
            <DropDownButton primary={true} dropdown={shareTargets.map(target => <>{target.icon}{target.name}</>)} onClick={handleShare} onDropdownSelected={handleShare}><>{shareTargets[0].icon}{shareTargets[0].name}</></DropDownButton>
            <button onClick={copy}><RiClipboardLine />Copy</button>
            <button onClick={updateData}><RiRefreshLine />Update data…</button>
            <button onClick={clearLog}><RiDeleteBin2Line />Clear log…</button>
            <button onClick={debugCSV}>Debug CSV</button>
            {visualPreviews.length > 0 && <DropDownButton dropdown={visualPreviews.map(vis => <>{vis.icon}{vis.name}</>)} onClick={() => visualise(-1)} onDropdownSelected={index => visualise(index)}>{visualisation ? "Close " + visualisation.name : <>{visualPreviews[0].icon}{visualPreviews[0].name}</>}</DropDownButton>}
          </div>
          <p id="info">
            This is the data on your micro:bit. To analyse it and create your own graphs, transfer it to your computer. You can copy and paste your data, or download it as a CSV file which you can import into a spreadsheet or graphing tool. <a href="https://microbit.org/get-started/user-guide/data-logging/" target="_blank">Learn more about micro:bit data logging</a>.
          </p>
          <div id="data">
            {visualisation && visualisation.generate(log)}
            <DataLogTable log={log} highlightDiscontinuousTimes={visualisation === LineGraphVisualisation} />
          </div>
        </main>
      </div>
    </IconContext.Provider>
  );
}

export default App;
