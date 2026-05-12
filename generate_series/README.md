# Generate Series Extension

`generate_series` procedure extension for Valentina.

## Build

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target generate_series -j
```

For multi-config generators, such as Visual Studio or Ninja Multi-Config:

```sh
cmake -S . -B build
cmake --build build --config Release --target generate_series -j
```

The build creates a zip package:

```text
build/generate_series.zip
```

## Procedure

```text
CALL generate_series(start, stop, step)
SELECT * FROM generate_series(start, stop, step)
```

- `start`: integer or double start value.
- `stop`: integer or double end value.
- `step`: optional increment, default `1` or `1.0`.
