# LogFS HTML Resources
This folder contains the HTML+JS used by MicroBitLogFS. 

## Requirements
 - The overall HTML+JS MUST be byte aligned to 2048 bytes. 
 - Use trailing spaces to ensure the file to 2048 bytes.
 - The file must end with the delimiting characters `<!--FS_START`

## Installation and Updating
This source code is not automatically compiled into the resulting CODAL binary.
To update builds, follow the process below:

 - Update and test the HTML+JS content. This can be achieved by appending the sample-trailer.txt file to a copy of header.html. This simulates a valid LogFS filesystem.
 - Minify the HTML_JS to reduce code size
 - Copy the 2048 byte HTML+JS into a TEXT to HEX converter, such as https://onlinehextools.com/convert-text-to-hex. Ensure the resulting digits are prepended with `0x`
 - Ensure the resulting output is comma sepaated. If not, use a search/replace tool to replace spaces with commas to generate output in the form `0x48,0x65,0x6c,0x6c,0x6f`
 - Copy/Paste this hex data into the initializer of the binary array called `MicroBitLog::header` in file `source/MicroBitLog.cpp` in this repo.
