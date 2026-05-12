# Base64 Extension

Base64 extension for Valentina.

## Build

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target base64 -j
```

For multi-config generators, such as Visual Studio or Ninja Multi-Config:

```sh
cmake -S . -B build
cmake --build build --config Release --target base64 -j
```

The build creates a zip package:

```text
build/base64.zip
```

## Functions

```text
base64encode(data)
base64decode(data)
```

- `base64encode`: converts binary data into Base64-encoded text.
- `base64decode`: converts Base64-encoded text back into binary data.
