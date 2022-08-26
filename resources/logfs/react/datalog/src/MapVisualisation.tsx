import { DataLog, VisualisationType } from "./App";
import { ReactNode } from "react";
import React from "react";
import "./MapVisualisation.css";
import Plot from "react-plotly.js";
import { Data, Layout, PlotData } from "plotly.js";

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
    renderRow: number;
    children: React.ReactNode;
}

class LogValueTooltip extends React.Component<LogValueTooltipProps, { absX: number, absY: number }> {

    constructor(props: LogValueTooltipProps) {
        super(props);

        this.state = {
            absX: -1,
            absY: -1
        }
    }

    componentDidMount() {
        window.addEventListener('mousemove', this.handleMouseMove);
    }

    componentWillUnmount() {
        window.removeEventListener('mousemove', this.handleMouseMove);
    }

    handleMouseMove = (event: MouseEvent) => {
        this.setState(
            { absX: event.clientX, absY: event.clientY }
        );
    }

    render() {
        return (
            <div>
                {this.props.children}
                {this.props.renderRow > -1 &&
                    <div className="log-tooltip" style={{ left: this.state.absX, top: this.state.absY }}>
                        <table>
                            <tbody>
                                <tr>
                                    {Object.keys(this.props.log).map(header => <td>{header}</td>)}
                                </tr>
                                <tr>
                                    {Object.keys(this.props.log).map(header => <td>{this.props.log[header][this.props.renderRow]}</td>)}
                                </tr>
                            </tbody>
                        </table>
                        tooltip tooltip {this.props.renderRow}
                    </div>
                }
            </div>
        )
    }
}

interface MapViewProps {
    log: DataLog
}

class MapView extends React.Component<MapViewProps, { selectedRow: number }> {

    constructor(props: MapViewProps) {
        super(props);

        this.state = {
            selectedRow: -1
        }
    }

    render() {
        const headings = Object.keys(this.props.log);

        const lats = this.props.log[latitudeColumn].map(elem => Number(elem));
        const lons = this.props.log[longitudeColumn].map(elem => Number(elem));

        // zip the latitude and longitude arrays into an array of lat/long pairs
        const latLonPairs: [number, number][] = this.props.log[latitudeColumn].map((elem, index) => [Number(elem), Number(this.props.log[longitudeColumn][index])]);

        const geoJson = toGeoJson(this.props.log);

        const data: Data = {
            type: "scattermapbox",
            lat: lats,
            lon: lons,
            text: lats.map((_, index) => headings.filter(header => header !== latitudeColumn && header !== longitudeColumn).map(heading => heading + ": " + this.props.log[heading][index]).join(", ")),
            marker: { color: "fuchsia", size: 4 }
        };


        const layout: Partial<Layout> = {
            dragmode: "zoom",
            mapbox: {
                style: "open-street-map",
                center: { lat: lats[0], lon: lons[0] }, zoom: 3
            },
            width: 800,
            height: 600,
            title: "Test 2",
            margin: { r: 0, t: 0, b: 0, l: 0 }
        };

        return (
            <Plot data={[data]} layout={layout} config={{ displaylogo: false, responsive: true, toImageButtonOptions: { filename: "MY_DATA" }, modeBarButtonsToRemove: ["select2d", "lasso2d", "autoScale2d"] }} />
        );

        /*return (
            <LogValueTooltip log={this.props.log} renderRow={this.state.selectedRow}>
                <Map height={300} defaultCenter={[50.879, 4.6997]} defaultZoom={11}>
                    {latLongPairs.map((latLong, index) => <Marker width={50} anchor={latLong} key={index} />)}
                    <Marker width={50} anchor={[0, 0]} />
                    <GeoJson>
                        {geoJson.map((feature, index) => <GeoJsonFeature feature={feature} key={index} onMouseOver={() => { this.handleMarkerHover(index) }} onMouseOut={this.handleMarkerLeave} />)}
                    </GeoJson>
                </Map>
            </LogValueTooltip>
        );*/
    }

    handleMarkerHover = (index: number) => {
        this.setState({ selectedRow: index });
    }

    handleMarkerLeave = () => {
        this.setState({ selectedRow: -1 });
    }
}

/*
        <Map height={300} defaultCenter={[50.879, 4.6997]} defaultZoom={11}>
          <Marker width={50} anchor={[0, 0]} />
        </Map> */

const MapVisualisation: VisualisationType = {
    name: "Map",
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
        if ([...log[lats], ...log[lngs]].every(elem => isNaN(Number(elem)))) {
            return "Latitude and Longitude fields need to be numeric.";
        }

        return null;
    },
    generate: log => {
        return (<MapView log={log} />);
    }
};

export default MapVisualisation;