import { timestampRegex, visualisationConfig, VisualisationType } from "./App";
import { ReactNode, useEffect, useState } from "react";
import React from "react";
import "./MapVisualisation.css";
import Plot from "react-plotly.js";
import { Data, Layout, PlotData } from "plotly.js";
import DataLog from "./DataLog";
import { lstat } from "fs";
import { RiMap2Line, RiMapLine } from "react-icons/ri";

const latitudeColumn = "Latitude";
const longitudeColumn = "Longitude";

interface MapViewProps {
    log: DataLog
}

function MapView(props: MapViewProps) {

    const [selectedRow, setSelectedRow] = useState(-1);
    const [mapConsent, setMapConsent] = useState(() => window.localStorage.getItem("open-street-map-consent") === "true");
    const [currentTime, setCurrentTime] = useState(0);

    const [latsCol] = useState(() => props.log.dataForHeader(latitudeColumn).map(elem => Number(elem)));
    const [lonsCol] = useState(() => props.log.dataForHeader(longitudeColumn).map(elem => Number(elem)));
    const [timeCol] = useState(() => (props.log.dataForHeader(timestampRegex) || [0]).map(elem => Number(elem)));

    const firstOverTime = timeCol.findIndex(elem => Number(elem) >= currentTime);
    const maxTime = timeCol[timeCol.length - 1];

    const lats = latsCol.slice(0, firstOverTime);
    const lons = lonsCol.slice(0, firstOverTime);

    const tween = (start: number, end: number, value: number) => start + (value * (end - start));

    let nextLat = latsCol[firstOverTime];
    let nextLon = lonsCol[firstOverTime];

    const prevTime = firstOverTime === 0 ? 0 : timeCol[firstOverTime - 1];
    const nextTime = timeCol[firstOverTime];
    const timeProgress = (currentTime - prevTime) / (nextTime - prevTime);

    nextLat = tween(lats[lats.length - 1], nextLat, timeProgress);
    nextLon = tween(lons[lons.length - 1], nextLon, timeProgress);

    const [layout] = useState<Partial<Layout>>(() => ({
        dragmode: "zoom",
        mapbox: {
            style: "open-street-map",
            center: { lat: latsCol.length === 0 ? 0 : latsCol[0], lon: lonsCol.length === 0 ? 0 : lonsCol[0] },
            zoom: 16
        },
        height: 500,
        margin: { r: 0, t: 0, b: 0, l: 0 },
        uirevision: "true"
    }));

    const handleMarkerHover = (index: number) => {
        setSelectedRow(index);
    }

    const handleMarkerLeave = () => {
        setSelectedRow(-1);
    }

    /*useEffect(() => {
        const timer = setInterval(() => setCurrentTime(prev => prev + 0.05), 50);
        return () => clearInterval(timer);
    });*/

    if (!mapConsent) {
        return (
            <div id="map-privacy-notice">
                <div>
                    <h3>Using the map view requires accessing data from OpenStreetMap</h3>
                    You can view their privacy policy <a href="https://wiki.osmfoundation.org/wiki/Privacy_Policy">here</a>.
                </div>

                <div className="modal-buttons">
                    <button onClick={() => {
                        window.localStorage.setItem("open-street-map-consent", "true");
                        setMapConsent(true);
                    }}>I'm OK with this</button>
                </div>
            </div>
        );
    }

    const data: Data = {
        type: "scattermapbox",
        lat: [...lats, nextLat],
        lon: [...lons, nextLon],
        text: lats.map((_, index) => props.log.headers.filter(header => header !== latitudeColumn && header !== longitudeColumn).map(heading => heading + ": " + props.log.dataForHeader(heading)[index]).join(", ")),
        marker: { color: "fuchsia", size: 9 },
        mode: "lines+markers",
        line: {
            width: 3,
            color: [...Array(Math.max(lats.length - 1, 0)).fill("red"), "green"]
        }
    };

    return (
        <>
            <Plot className="graph" data={[data]} layout={layout} config={visualisationConfig} />
            <div>{currentTime}</div>
            <input style={{width: "100%"}} type="range" min={0} max={maxTime * 1000} value={currentTime * 1000} onChange={(e) => setCurrentTime(Number(e.target.value) / 1000)} />
        </>
    );
}

const MapVisualisation: VisualisationType = {
    name: "Map",
    icon: <RiMap2Line />,
    availablityError: log => {
        const lats = log.dataForHeader(latitudeColumn);
        const lngs = log.dataForHeader(longitudeColumn);

        if (lats.length === 0 || lngs.length === 0) {
            return "Latitude and Longitude columns are required.";
        }

        if (lats.length !== lngs.length) {
            return "Latitude and Longitude columns need to be the same length.";
        }

        // check all latitude and longitude values at once
        if ([...lats, ...lngs].every(elem => isNaN(Number(elem)))) {
            return "Latitude and Longitude fields need to be numeric.";
        }

        return null;
    },
    generate: log => {
        return (<MapView log={log} />);
    }
};

export default MapVisualisation;