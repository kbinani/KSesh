# KSesh

An Interactive Hieroglyphic Editor, available on Windows, macOS, and the Web.

[![Actions Status](https://github.com/kbinani/KSesh/workflows/CI/badge.svg)](https://github.com/kbinani/KSesh/actions)
[![License](https://img.shields.io/github/license/kbinani/KSesh)](https://www.gnu.org/licenses/agpl-3.0.html)

![screenshot](https://raw.githubusercontent.com/kbinani/KSesh/main/pkg/app-store/screenshot-en.png)

# Install

- Windows
  - https://www.microsoft.com/store/apps/9PPDNN5S6Z36
- macOS
  - https://apps.apple.com/us/app/ksesh/id6596728393
- Web
  - https://kbinani.github.io/KSesh/

# Build

- Windows
  ```
  cmake -B build
  cmake --build ./build --target ksesh --config Release --parallel
  ./build/ksesh_artifacts/Release/KSesh.exe
  ```

- macOS
  ```
  cmake -B build -G Xcode
  cmake --build ./build --target ksesh --config Release --parallel
  open ./build/ksesh_artefacts/Release/KSesh.app
  ```

- iOS
  ```
  cmake -B build -G Xcode -DCMAKE_SYSTEM_NAME=iOS
  open build/KSesh.xcodeproj
  # Choose the target "ksesh" then run on the simulator or on your device.
  ```

- Web
  ```
  npm i
  make web
  open ./public/index.html
  ```

# SAST Tools

[PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
