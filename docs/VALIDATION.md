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

## Observed CI result

Passed: [target-platform build/check](https://github.com/netcodermike/pixel-sprite-workshop/actions/runs/42204087555). Source commit: `123bef3ba1b16cd3e0630dc453c3316bcceb8815`. Later documentation-only changes do not alter the tested implementation. The reproducible commands above identify test scope; native GUI/audio/permissions are not implied by compilation.
