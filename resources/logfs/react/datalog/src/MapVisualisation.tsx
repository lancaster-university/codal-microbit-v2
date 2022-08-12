import Plot from "react-plotly.js";
import { Data } from 'plotly.js';
import { DataLog, VisualisationType } from "./App";
import { Map, Marker, GeoJson, Point, GeoJsonFeature } from "pigeon-maps";
import { ReactNode } from "react";
import React from "react";

const latitudeColumn = "Latitude";
const longitudeColumn = "Longitude";

function toGeoJson(log: DataLog) {
    let features = [];

    for (let i = 0; i < log.Latitude.length; i++) {
        features.push({
            type: "Feature",
            geometry: {
                type: "Point",
                coordinates: [Number(log[longitudeColumn][i]), Number(log[latitudeColumn][i])]
            }
        });
    }

    return features;
}

interface LogValueTooltipProps {
    log: DataLog;
    renderRow?: number | null;
    children: React.ReactNode;
}

class LogValueTooltip extends React.Component<LogValueTooltipProps, {}> {

    constructor(props: LogValueTooltipProps) {
        super(props);
    }

    render() {
        return (<div>
            <div className="log-tooltip">tooltip tooltip {this.props.renderRow}</div>
            {this.props.children}
        </div>)
    }
}

/*
        <Map height={300} defaultCenter={[50.879, 4.6997]} defaultZoom={11}>
          <Marker width={50} anchor={[0, 0]} />
        </Map> */

const MapVisualisation: VisualisationType = {
    name: "Line Graph",
    availablityError: log => {
        const headings = Object.keys(log);

        const lats = headings.find(heading => heading === latitudeColumn);
        const lngs = headings.find(heading => heading === longitudeColumn);

        if (!lats || !lngs) {
            return "Latitude and Longitude columns are required.";
        }

        if (log[latitudeColumn].length !== log[longitudeColumn].length) {
            return "Latitude and Longitude columns need to be the same length.";
        }

        // check all latitude and longitude values at once
        if ([...log[lats], ...log[lngs]].every(elem => Number(elem) !== NaN)) {
            return "Latitude and Longitude fields need to be numeric.";
        }

        if (headings.length < 2) {
            return "Requires two or more columns. Timestamps must be enabled.";
        }

        return null;
    },
    generate: log => {
        const headings = Object.keys(log);

        // zip the latitude and longitude arrays into an array of lat/long pairs
        const latLongPairs: [number, number][] = log[latitudeColumn].map((elem, index) => [Number(elem), Number(log[longitudeColumn][index])]);

        const geoJson = toGeoJson(log);

        const [selectedRow, setSelectedRow] = React.useState(0);

        function handleMarkerClick(rowIndex: number) {
            console.log("handle", rowIndex);
            setSelectedRow(rowIndex);
        }


        return (
            <LogValueTooltip log={log} renderRow={selectedRow} key={selectedRow}>
                <Map height={300} defaultCenter={[50.879, 4.6997]} defaultZoom={11}>
                    {latLongPairs.map((latLong, index) => <Marker width={50} anchor={latLong} key={index} />)}
                    <Marker width={50} anchor={[0, 0]} />
                    <GeoJson>
                        {geoJson.map((feature, index) => <GeoJsonFeature feature={feature} key={index} onClick={() => { handleMarkerClick(index) }} />)}
                    </GeoJson>
                </Map>
            </LogValueTooltip>
        );
    }
};

export default MapVisualisation;