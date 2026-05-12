# Compress Extension

Compression extension for Valentina.

## Build

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target compress -j
```

For multi-config generators, such as Visual Studio or Ninja Multi-Config:

```sh
cmake -S . -B build
cmake --build build --config Release --target compress -j
```

The build fetches and links zlib 1.3.1 statically.

The build creates a zip package:

```text
build/compress.zip
```

## Functions

```text
compress(data)
uncompress(data)
```

- `compress`: compresses binary data and returns a BLOB with the compressed size prepended.
- `uncompress`: restores binary data produced by `compress`.
