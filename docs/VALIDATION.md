# Validation

Authoring date: November 6, 2026. Host: Windows. Target: Windows, macOS, Linux desktop with Qt 6.

## Local result

Qt/C++ unavailable on authoring host. CI builds native Qt code and runs image/model checks headlessly.

## Reproducible checks

The GitHub Actions workflow builds/runs the following on `ubuntu-latest`:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Manual acceptance

Paint/erase, duplicate/select/delete frames, play/stop, save/reopen a project, reject malformed JSON, and inspect PNG dimensions/transparency.

A workflow file is not evidence of a passing run. Observed CI run links and source commits are recorded after publication. Native GUI, sound, sleep/resume, and platform permissions require direct target-platform acceptance even when model tests pass.
