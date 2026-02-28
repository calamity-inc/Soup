# Soup CLI

The Soup CLI offers many tools to test and make use of functionality provided in the Soup library, including:

- Keyboard and gamepad visualisation (`soup keyboard`, `soup gamepad`)
- WASM runtime with WASI support (`soup wasm <file> [args...]`)
- GeoIP lookup (`soup geoip <ip>`)
- QR code generator (`soup qr <contents>`)

## Building

The CLI can be built in the same ways as Soup:
- **Visual Studio**
- **PHP Build Scripts** — `php build_lib.php && php build_cli.php`
- **Sun** — simply run `sun` in this directory

## Prebuilt Binaries

The CLI is included in the release artefacts of [the build workflow](<https://github.com/calamity-inc/Soup/actions/workflows/build.yml>) if you prefer downloading your executables.
