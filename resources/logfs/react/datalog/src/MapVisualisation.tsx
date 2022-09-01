import { visualisationConfig, VisualisationType } from "./App";
import { ReactNode } from "react";
import React from "react";
import "./MapVisualisation.css";
import Plot from "react-plotly.js";
import { Data, Layout, PlotData } from "plotly.js";
import DataLog from "./DataLog";
import { lstat } from "fs";

const latitudeColumn = "Latitude";
const longitudeColumn = "Longitude";

interface MapViewProps {
    log: DataLog
}

class MapView extends React.Component<MapViewProps, { selectedRow: number, openStreetMapConsent: boolean }> {

    constructor(props: MapViewProps) {
        super(props);

        this.state = {
            selectedRow: -1,
            openStreetMapConsent: window.localStorage.getItem("open-street-map-consent") === "true"
        }
    }

    render() {
        if (!this.state.openStreetMapConsent) {
            return (
                <div id="map-privacy-notice">
                    <div>
                        <h3>Using the map view requires accessing data from OpenStreetMap</h3>
                        You can view their privacy policy <a href="https://wiki.osmfoundation.org/wiki/Privacy_Policy">here</a>.
                    </div>

                    <div className="modal-buttons">
                        <button onClick={() => {
                            window.localStorage.setItem("open-street-map-consent", "true");
                            this.setState({ openStreetMapConsent: true });
                        }}>I'm OK with this</button>
                    </div>
                </div>
            );
        }

        const lats = this.props.log.dataForHeader(latitudeColumn).map(elem => Number(elem));
        const lons = this.props.log.dataForHeader(longitudeColumn).map(elem => Number(elem));

        const data: Data = {
            type: "scattermapbox",
            lat: lats,
            lon: lons,
            text: lats.map((_, index) => this.props.log.headers.filter(header => header !== latitudeColumn && header !== longitudeColumn).map(heading => heading + ": " + this.props.log.dataForHeader(heading)[index]).join(", ")),
            marker: { color: "fuchsia", size: 9 },
            mode: "lines+markers",
            line: {
                width: 1,
                color: "red"
            }
        };


        const layout: Partial<Layout> = {
            dragmode: "zoom",
            mapbox: {
                style: "open-street-map",
                center: { lat: lats[0], lon: lons[0] }, zoom: 16
            },
            height: 500,
            margin: { r: 0, t: 0, b: 0, l: 0 }
        };

        return (
            <Plot className="graph" data={[data]} layout={layout} config={visualisationConfig} />
        );
    }

    handleMarkerHover = (index: number) => {
        this.setState({ selectedRow: index });
    }

    handleMarkerLeave = () => {
        this.setState({ selectedRow: -1 });
    }
}

const MapVisualisation: VisualisationType = {
    name: "Map",
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