# JPEG Analyzer

This project analyzes JPEG files.

## Scope

- [ ] Baseline (`SOF0`)
- [ ] Progressive (`SOF2`)

## Features

- [x] Reads JPEG files and shows all markers used in the file.
- [x] Analyzes and visualizes Huffman and quantization tables.
- [ ] Converts supported JPEG files into Bitmap files.

## JPEG Markers

|Marker|Meaning|Required|Values|
|------|--------|--------|-----------|
|`SOI`|*Start Of Image*|Yes|`0xFF D8`|
|`EOI`|*End Of Image*|Yes|`0xFF D9`|
|`SOS`|*Start Of Scan*|Yes|`0xFF D8`|
|`DHT`|*Define Huffman Table*|Yes|`0xFF C4`|
|`DQT`|*Define Quantization Table*|Yes|`0xFF DB`|

## Build

TODO: Cross Platform

- Visual Studio (Windows)
- Linux (Debian)
- Mac

### On Windows

1. Clone the repository:  `git clone https://github.com/pell2401/Jpeg-Analyzer.git`.
2. Open `Jpeg-Analyzer.sln` in Visual Studio.
3. Select the `x64` configuration.
4. Build the solution, or press `F5` to build and run the project.

### On Linux

TODO:

### On Mac OS

TODO:

## Directory Structure

```txt
├── Jpeg-Analyzer.sln
├── Jpeg-Analyzer.vcxproj
├── Jpeg-Analyzer.vcxproj.filters
├── Jpeg-Analyzer.vcxproj.user
├── README.md
├── include
│   ├── AppData.h
│   ├── AppImage.h
│   └── AppImageJpeg.h
└── src
    ├── AppData.cpp
    ├── AppImage.cpp
    ├── AppImageJpeg.cpp
    └── main.cpp
```

### Project Files for Visual Studio (Windows)

|File|Description|
|----|-----------|
|`Jpeg-Analyzer.sln`|Solution File|
|`Jpeg-Analyzer.vcxproj`|Visual C Project|
|`Jpeg-Analyzer.vcxproj.filters`|Visual C Project Filters / **Solution Explorer**|
|`Jpeg-Analyzer.vcxproj.user`|User-specific debugging and build settings|

### Document Files

#### `README.md`

This file is written in Markdown and is exactly what you're reading now.

### Header and Source Files

#### `AppData.h` and `AppData.cpp`

The header file contains the declaration of all structure types used in the program.
The header file also defines the class **`CAppData`**, which has necessary objects that can be accessed through its static pointer to the class instance.

#### `AppImage.h` and `AppImage.cpp`

These files contain the declarations and definitions of the class **`CAppImage`**, which provides instance methods such as **`ReadFile`**.

#### `AppImageJpeg.h` and `AppImageJpeg.cpp`

These files contain the declarations and definitions of the class **`CAppImageJpeg`**, which provides instance methods such as **`ParseMarker`**.

## References

- [Exploring JPEG](https://www.imperialviolet.org/binary/jpeg/)
- [API Specifications.md](./docs/API%20Specifications.md)
