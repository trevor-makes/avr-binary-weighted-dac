# Arduino Binary-Weighted DAC

- 2 channels of fast 6-bit analog output with and Arduino Nano and 12 resistors
- Display 64x64 pixel bitmaps and vector animations on an oscilloscope

TODO

Distributed under the [MIT license](LICENSE.txt)

## Building the demo

Use the [PlatformIO](https://platformio.org/) plugin for [VSCode](https://code.visualstudio.com/).

Open the project folder with VSCode, select the environment for your board (`uno`, `nano`, `oldnano`), and click `Upload`.

## Assembling the circuit

TODO

## Using the demo

After building and uploading the program to the Arduino, connect a serial monitor such as the one included with PlatformIO. A '>' should appear as a prompt for input. The following commands are available:

```
>attract
```
Cycle between several different display modes with preset time intervals between each.

```
>logo
```
Display "Trevor Makes!" logo in bitmap mode.

```
>clear
```
Clears the bitmap display.

```
>print "text"
```
Scroll bitmap up one line and print given text.

```
>maze
```
Display scrolling randomized maze using PETSCII characters.

```
>circle
```
Display vector circle (cosine in X and sine in Y).

```
>cross
```
Display vector cross (triangle wave in X and sawtooth wave in Y).

```
>bounce
```
Display bouncing ball vector animation.

```
>circum
```
Display random circumscribed triangles animation.

```
>doge
```
Display doge bitmap.

![](bitmaps/doge.png)

```
>pepe
```
Display pepe bitmap.

![](bitmaps/pepe.png)

```
>fliph
```
Flip bitmap display horizontally.

```
>flipv
```
Flip bitmap display vertically.

```
>export
```
Capture current bitmap display in [IHX](https://en.wikipedia.org/wiki/Intel_HEX) format and print to terminal.

```
>import
```
Read [IHX](https://en.wikipedia.org/wiki/Intel_HEX) formatted string from terminal and unpack into bitmap display. Copy-paste IHX from `>export` command or `bitmaps/convert.py` script.

```
>save [index]
```
Store current bitmap in EEPROM. Arduino Uno/Nano can hold 2 64x64 bitmaps in EEPROM, at indices 0 and 1 (defaults to 0 if not given).

```
>load
```
Load bitmap display from EEPROM. Arduino Uno/Nano can hold 2 64x64 bitmaps in EEPROM, at indices 0 and 1 (defaults to 0 if not given).

```
>delay [microseconds]
```
Number of microseconds to linger on each set pixel in bitmap mode. Larger numbers make the display sharper, but at reduced frame rate.

## Dependencies

The following dependencies will be downloaded by PlatformIO at build time:

- [core](https://github.com/trevor-makes/core)

## Contributors

[Trevor Makes](mailto:the.trevor.makes@gmail.com)
