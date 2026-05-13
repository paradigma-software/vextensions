# ValentinaDB Extensions

Starting with version 16, ValentinaDB now supports extensions written in C, C++, Python, and JavaScript. 
This repository contains various extensions that can be downloaded in compiled form 
and installed into your ValentinaDB application or Valentina Server.

- [Docs](https://valentina-db.com/dokuwiki/doku.php?id=valentina:products:vcomponents:vkernel:extensions:extensions)


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
