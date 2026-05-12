# Valentina extensions

Valentina extension modules.

## Layout

```text
cmake/                  shared CMake helpers
include/                shared extension API headers
ocr/                    OCR extension
```

## Build All Extensions

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target package_extensions -j
```

For multi-config generators, such as Visual Studio or Ninja Multi-Config:

```sh
cmake -S . -B build
cmake --build build --config Release --target package_extensions -j
```

Each extension produces its own zip package under `build/`, for example:

```text
build/ocr.zip
```

## Extensions

- [OCR](ocr/README.md)
