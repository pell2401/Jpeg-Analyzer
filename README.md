# JPEG Analyzer

This project analyzes arbitrary JPEG files.

## Features

- [x] Reads JPEG files and shows all markers used in the file.
- [ ] Analyzes and visualizes huffman and quantization tables.
- [ ] Converts arbitrary JPEG files to Bitmap files.

## JPEG Markers

|Marker|Meaning|Required|Values|
|------|--------|--------|-----------|
|`SOI`|*Start Of Image*|Yes|`0xFF D8`|
|`EOI`|*End Of Image*|Yes|`0xFF D9`|
|`SOS`|*Start Of Scan*|Yes|`0xFF D8`|
|`DHT`|*Define Huffman Table*|Yes|`0xFF C4`|
|`DQT`|*Define Quantization Table*|Yes|`0xFF DB`|

## Build

### Requirements

- Visual Studio 2022 (or later)
- Windows 11

### How To Build

1. Clone the repository:  `git clone https://github.com/pell2401/Jpeg-Analyzer.git`.
2. Open `Jpeg-Analyzer.sln` in Visual Studio.
3. Select the `x64` configuration.
4. Build the solution, or press `F5` to build and run the project.

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

### Project Files

#### `Jpeg-Analyzer.sln`

This file specifies how this project was created.

```txt
VisualStudioVersion = 17.13.35931.197 d17.13
MinimumVisualStudioVersion = 10.0.40219.1
```

#### `Jpeg-Analyzer.vcxproj`

This file is in XML and specifies the project configuration, source files, and build settings.

#### `Jpeg-Analyzer.vcxproj.filters`

This file is in XML format and manages the Solution Explorer.

#### `Jpeg-Analyzer.vxcproj.user`

This file is in XML format and stores user-specific debugging and build settings. This file is not tracked by Git and not necessary for building.

### Document Files

#### `README.md`

This file is written in Markdown and is exactly what you're reading now.

### Header and Source Files

#### `AppData.h` and `AppData.cpp`

The header file contains the declaration of all structure types used in the program.
The header file also defines the class **`CAppData`**, which has necessary objects that can be accessed through its static pointer to the class instance.

The **`CAppData`** class is not fully utilized yet.

#### `AppImage.h` and `AppImage.cpp`

These files contain the declarations and definitions of the class **`CAppImage`**, which provides instance methods such as **`ReadFile`**.

#### `AppImageJpeg.h` and `AppImageJpeg.cpp`

These files contain the declarations and definitions of the class **`CAppImageJpeg`**, which provides instance methods such as **`ParseMarker`**.

## References

- [Exploring JPEG](https://www.imperialviolet.org/binary/jpeg/)
