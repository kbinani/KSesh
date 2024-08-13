# KSesh

An Interactive Hieroglyphic Editor, available on Windows, macOS, and the Web.

[![Actions Status](https://github.com/kbinani/KSesh/workflows/CI/badge.svg)](https://github.com/kbinani/KSesh/actions)
[![License](https://img.shields.io/github/license/kbinani/KSesh)](https://www.gnu.org/licenses/agpl-3.0.html)

![screenshot](https://raw.githubusercontent.com/kbinani/KSesh/main/pkg/app-store/screenshot.png)

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
  mkdir build
  cd build
  cmake ..
  cmake --build . --target ksesh --config Release --parallel
  ksesh_artifacts/Release/KSesh.exe
  ```

- macOS
  ```
  mkdir build
  cd build
  cmake .. -GXcode
  cmake --build . --target ksesh --config Release --parallel
  open ./ksesh_artefacts/Release/KSesh.app
  ```

- Web
  ```
  npm i
  make web
  open ./public/index.html
  ```
