# ocr

OCR extension for Valentina.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target ocr -j
```

For multi-config generators, such as Visual Studio or Ninja Multi-Config:

```sh
cmake -S . -B build
cmake --build build --config Release --target ocr -j
```

The build fetches and links these libraries statically:

- Tesseract 5.5.2
- Leptonica 1.86.0
- zlib 1.3.1
- libpng 1.6.58
- libjpeg-turbo 3.1.4.1

The resulting `ocr` module should not have dynamic dependencies on Tesseract,
Leptonica, zlib, libpng, or libjpeg.

The build also creates a zip package:

```text
build/ocr.zip
```

The archive contains the built module and the `tessdata/` directory.

## Runtime Data

The repository contains selected `*.traineddata` files in the source
`tessdata/` directory. The build copies them to `tessdata/` next to the built
`ocr` module. At runtime the extension uses that directory as Tesseract's data
path.

Bundled languages:

```text
ukr eng pol deu fra ita spa por chi_sim chi_tra jpn kor
```

Additional languages must be placed in the same `tessdata/` directory. The
default language is `eng`. Multiple languages can be used with Tesseract's
`+` syntax, for example `eng+ukr`.

## Functions

```text
ocr_text(data, lang, psm, oem)
ocr_html(data, lang, psm, oem)
ocr_tsv(data, lang, psm, oem)
```

- `data`: image bytes. PNG, JPEG, and BMP are supported by the current build.
- `lang`: optional Tesseract language code, default `eng`.
- `psm`: optional page segmentation mode, `0..13`, default `3`.
- `oem`: optional OCR engine mode, `0..3`, default `3`.

`ocr_text` returns plain UTF-8 text. `ocr_html` returns hOCR markup with layout
coordinates. `ocr_tsv` returns Tesseract TSV output with hierarchy, bounding
boxes, confidence, and text.
