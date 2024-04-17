/* To start the programme press button A
 * Then when the programme is done a smily face will be shown on the display
 * Unplug and replug the micro:bit to get the latest version of MY_DATA.HTM
 */

datalogger.onLogFull(function () {
    full_once = true
})
let full_once = false

// Wait until Button A is pressed
while (!(input.buttonIsPressed(Button.A))) {
    basic.showLeds(`
        . . # . .
        . # . . .
        # # # # #
        . # . . .
        . . # . .
        `)
}
datalogger.includeTimestamp(FlashLogTimeStampFormat.None)
full_once = false

basic.showIcon(IconNames.Meh)

// Fill the data logging storage
while (!(full_once)) {
    datalogger.log(
    datalogger.createCV("a", "Long text to fill the data storage quicker"),
    datalogger.createCV("b", "Even longer text to fill the data storage quicker"),
    datalogger.createCV("c", "Last long text to fill the data storage quicker")
    )
    basic.pause(10)
}

basic.showIcon(IconNames.Surprised)

// Now do a fast erase and write the data to be shown to the user
datalogger.deleteLog(datalogger.DeleteType.Fast)

datalogger.log(
    datalogger.createCV("a", 16),
    datalogger.createCV("b", 772),
    datalogger.createCV("c", -680)
)
datalogger.log(
    datalogger.createCV("a", 160),
    datalogger.createCV("b", 880),
    datalogger.createCV("c", -512)
)
datalogger.log(
    datalogger.createCV("a", 80),
    datalogger.createCV("b", 844),
    datalogger.createCV("c", -552)
)
datalogger.log(
    datalogger.createCV("a", -80),
    datalogger.createCV("b", 744),
    datalogger.createCV("c", -724)
)
datalogger.log(
    datalogger.createCV("a", -120),
    datalogger.createCV("b", 628),
    datalogger.createCV("c", -764)
)
datalogger.log(
    datalogger.createCV("a", 184),
    datalogger.createCV("b", 944),
    datalogger.createCV("c", -236)
)
datalogger.log(
    datalogger.createCV("a", -40),
    datalogger.createCV("b", 796),
    datalogger.createCV("c", -484)
)
datalogger.log(
    datalogger.createCV("a", -204),
    datalogger.createCV("b", 536),
    datalogger.createCV("c", -644)
)
datalogger.log(
    datalogger.createCV("a", -100),
    datalogger.createCV("b", 916),
    datalogger.createCV("c", -664)
)
datalogger.log(
    datalogger.createCV("a", 56),
    datalogger.createCV("b", 780),
    datalogger.createCV("c", -232)
)
datalogger.log(
    datalogger.createCV("a", 68),
    datalogger.createCV("b", 852),
    datalogger.createCV("c", -596)
)

basic.showIcon(IconNames.Happy)
