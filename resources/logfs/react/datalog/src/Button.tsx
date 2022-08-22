import React, { useState } from "react";
import "./Button.css";
import DropDownMenu from "./DropDownMenu";

export interface ButtonProps {
    dropdown?: string[],
    children: React.ReactNode,
    onDropdownSelected?: (index: number) => any;
}

function Button(props: ButtonProps) {
    const [dropdownOpen, setDropdownOpen] = useState(false);
    const [selectedIndex, setSelectedIndex] = useState(0);

    const handleBlur = (event: any) => {
        if (event.relatedTarget !== null && !event.currentTarget.contains(event.relatedTarget)) {
            setDropdownOpen(false);
        }
    }

    const validDropdown = props.dropdown && props.dropdown.length > 0;

    return (
        <div className="button-wrapper" onBlur={handleBlur}>
            <button className={"button-main " + (validDropdown ? "dropdown" : "")}>
                {props.children}
            </button>
            {validDropdown && 
            <button className="button-dropdown" onClick={() => setDropdownOpen(!dropdownOpen)}>
                V
            </button>
            }
            {validDropdown && dropdownOpen && <DropDownMenu items={props.dropdown || []} onSelected={props.onDropdownSelected}/>}
        </div>
    );
}

export default Button;