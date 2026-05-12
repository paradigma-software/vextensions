# ValentinaDB extensions

ValentinaDB extension modules.

## Layout

```text
cmake/                  shared CMake helpers
include/                shared extension API headers
base64/                 Base64 extension
compress/               compression extension
generate_series/        generate_series procedure extension
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

## Extensions

- [OCR](ocr/README.md)
- [Compress](compress/README.md)
- [Base64](base64/README.md)
- [Generate Series](generate_series/README.md)
