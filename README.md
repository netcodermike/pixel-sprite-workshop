# Pixel Sprite Workshop

Paint 16×16 pixel-art frames, use palettes and transparency, preview animation, and export editable projects or PNG sprite sheets.

[![Build and verify](https://github.com/netcodermike/pixel-sprite-workshop/actions/workflows/verify.yml/badge.svg)](https://github.com/netcodermike/pixel-sprite-workshop/actions/workflows/verify.yml)

**Language:** C++ · **Platform:** Windows, macOS, Linux desktop with Qt 6 · **App type:** Native desktop sprite/animation editor

First-release learning implementation, with explicit scope below. This is one independently runnable app, not a placeholder in a combined repository.

## Features

- Pixel canvas with checkerboard transparency, left-drag paint, and right-drag erase.
- Eight palette colors plus a custom color dialog.
- 1–64 frames, duplication/deletion, selection, and 150 ms animation preview.
- Validated JSON project save/open and horizontal PNG sheet export.

## Requirements

C++17, CMake 3.16+, Qt 6 Widgets development packages. On Ubuntu: sudo apt-get install qt6-base-dev. On Windows/macOS install Qt 6 and set CMAKE_PREFIX_PATH to its matching compiler kit.

## Quick start

```sh
git clone https://github.com/netcodermike/pixel-sprite-workshop.git
cd pixel-sprite-workshop
cmake -S . -B build
cmake --build build --config Release
# Linux/macOS single-config build:
./build/sprite
# Windows Visual Studio build:
.\build\Release\sprite.exe
```

Commands run from the repository root. Lines marked for a particular operating system apply only there. Language tools are prerequisites, not installed by the app. Close desktop windows normally; terminal/server apps stop with Ctrl+C unless their usage notes say otherwise.

## Example workflow

Paint a small character, duplicate the frame, change a few pixels, and play the animation. Save/open the project, then export PNG. The exported sheet should contain each frame side by side with transparency intact.

## Architecture, configuration, and data

`main.cpp` contains QImage-backed 16×16 frames, JSON encode/decode, sprite-sheet composition, a custom QWidget canvas, and Qt controls. Each JSON frame stores 256 eight-digit ARGB pixel values under version 1. QSaveFile commits project writes; imports are limited to 1 MiB and 64 frames. PNG sheet width is 16 × frame count, height 16. Animation uses QTimer and temporarily disables painting. CMake links Qt6::Widgets. --self-test uses QCoreApplication without a display and validates serialization, pixels, sheet dimensions/placement, PNG writing, and invalid input.

## Verification

Build with the quick-start command where needed, then run:

```sh
ctest --test-dir build -C Release --output-on-failure
```

Manual acceptance: Paint/erase, duplicate/select/delete frames, play/stop, save/reopen a project, reject malformed JSON, and inspect PNG dimensions/transparency.

See [validation record](docs/VALIDATION.md) for observed checks and their limits. CI results verify only what the workflow runs; they do not establish native GUI correctness or production readiness.

## Limitations and next steps

Fixed 16×16 resolution, no undo, layers, onion skin, arbitrary image import, GIF/video export, palette persistence, or per-frame duration. Unsaved state is in memory; closing the app does not prompt to save. Save project before closing or use the open-project discard prompt when switching files. Exported sheets have no timing metadata. Distribution requires Qt runtime libraries and Qt deployment tooling.

## Troubleshooting

If CMake cannot find Qt6, install Qt 6 Widgets and point CMAKE_PREFIX_PATH at the correct kit. Do not mix MSVC and MinGW builds. Single-config binaries are build/sprite; Visual Studio uses build/Release. PNG export errors usually indicate an unwritable output path. Stop animation before painting.

## Source map

- `CMakeLists.txt`
- `main.cpp`

## Development and license

Read [contributing](CONTRIBUTING.md) before changing the app, and [security/privacy](SECURITY.md) before supplying private data. Use synthetic fixtures for tests. No telemetry, paid API key, or account is required by the app itself. Dependencies, when present, are stated in Requirements.

[MIT license](LICENSE), copyright 2026 netcodermike.
