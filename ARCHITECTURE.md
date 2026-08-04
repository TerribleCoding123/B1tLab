# Before Reading...

I already know that most of my string-based functions use expensive pass-by-value and return-by-value methods. This was done deliberately for two reasons:

1. As a beginner, I was uncomfortable with references.
2. The CLI tool was not meant to be ultra-fast or performant, because console printing itself is rather slow, and for a user, a few milliseconds' difference wouldn't mean much.

With that being said, I know that for read-only viewing objects like `std::string\_view`, if the object being viewed isn't modified or destroyed, then there's no need to worry about Undefined Behavior.

In addition, I admit that my architecture is rather unconventional, overly complicated, and awkward in places. I will patch all of the above in future updates before implementing new features.

# Table of Contents

* [1. Background \& Layout](#1-background--layout)

  * [1.1 History of Creation](#11-history-of-creation)
  * [1.2 Project Structure](#12-project-structure)
* [2. Input Handling \& Data Processing](#2-input-handling--data-processing)

  * [2.1 Character Filtering](#21-character-filtering)
  * [2.2 Initial Pipeline](#22-initial-pipeline)
  * [2.3 Event Flags](#23-event-flags)
* [3. Values](#3-values)

  * [3.1 Value Types (`mValues` \& `rValues`)](#31-value-types-mvalues-rvalues)
  * [3.2 Value Parsing \& Validation](#32-value-parsing-validation)
  * [3.3 The `value` Struct](#33-the-value-struct)
  * [3.4 Boolean Sign](#34-boolean-sign)
  * [3.5 Internal Structs](#35-internal-structs)
* [4. Conversion Engines](#4-conversion-engines)

  * [4.1 The Default Converter](#41-the-default-converter)
  * [4.2 The Standard Converter](#42-the-standard-converter)

    * [4.2.1 Modes](#421-modes)
    * [4.2.2 Resize Flags](#422-resize-flags)
    * [4.2.3 Input Handling Pipeline](#423-input-handling-pipeline)
* [5. Command Specifications](#5-command-specifications)

  * [5.1 Utility Commands](#51-utility-commands)
  * [5.2 Default Conversion Commands](#52-default-conversion-commands)
  * [5.3 Standard Conversion Commands](#53-standard-conversion-commands)

    * [5.3.1 Value Mode](#531-value-mode)
    * [5.3.2 Set Format Mode](#532-set-format-mode)
    * [5.3.3 Show Format Mode](#533-show-format-mode)
    * [5.3.4 Set Default Mode](#534-set-default-mode)
* [6. Diagnostic Message Specifications](#6-diagnostic-message-specifications)

  * [6.1 `built\_ins`](#61-built_ins)
  * [6.2 `binary`](#62-binary)

<h1 id="1-background--layout">Background \& Layout</h1>

<h2 id="11-history-of-creation">1.1 History of Creation</h2>

The idea of a binary converter as a starter project came from Python. Back then, I was actively learning its syntax and getting into programming by reading the book "A Byte of Python". As a first non-trivial project, I decided to go with a simple binary converter. The same happened when I started learning C++. At first, the converter was a simple function that accepted an integer input and returned the binary representation, but slowly I added more features, and soon it supported fractions as well.

I decided to keep pushing and added periodic fraction support (essentially finishing the Default Converter). Thanks to learncpp.com, I tried to keep the project as organized as possible without polluting the global namespace, creating separate files and header guards. This allowed me to add complexity without the fear of getting an overly messy project, and after dealing with periodic fractions, I added formats.

For format support, I created a new type of converter (the Standard Converter) and made it digest special flags to change preset formats, view them, reset them, or manipulate input values. After deciding to halt adding any other technical features, I decided to improve how the program looked. Using the [replxx](https://github.com/AmokHuginnsson/replxx) library, I implemented syntax highlighting and formatted diagnostic messages. As of now, I am refining and improving what I have already written while gradually building out new features.

<h2 id="12-project-structure">1.2 Project Structure </h2>

B1tLab consists of four primary components: `built\_ins`, `binary`, `view`, and `main`.

### `built\_ins`

`built\_ins` is a core component of the converter. It consists of utility functions for input parsing, casting, validation, formatting, and core diagnostic messages. Anything that a converter tool needs for its input and output is located in this folder.

### `binary`

`binary` is the component where all conversion-related functions are stored. This directory consists of two converter engines: `DEFAULT` and `STANDARD`. The directory also hosts all of the utilities that these engines use, such as formats, diagnostic message functions, specialized converter functions, and specific input validators.

### `view`

The `view` module is used for formatting and styling purposes. In it, there are syntax highlighting functions, an RGB666 color palette, and ANSI escape codes for toggling text formatting.

### `main`

`main` is the program's entry module. The only function it has is `CommandCheck()`, which calls a specific command based on user input.

<h1 id="2-input-handling--data-processing">2. Input Handling \& Data Processing</h1>

<h2 id="21-character-filtering">2.1 Character Filtering</h2>

Before the input handling procedure can start, user input should be filtered to prevent Undefined Behavior and potential crashes. B1tLab only supports the ASCII character set. Any wide character will break assumptions about the length of the string and make the program unsafe. To prevent this, the `built\_ins` module has a `utf\_filter` function. The function alters the user input dynamically through `replxx`'s `set\_modify\_callback()` method, replacing all wide characters with the `'?'` symbol.

### How It Works

It is known that `std::string` is essentially an array of `char`, which occupy a single byte in memory. Any wide UTF character cannot fit into a single `char` type; hence, it is converted into multiple UTF-8 bytes. According to encoding rules, any such multi-byte UTF-8 character is guaranteed to have `'1'` as a leading bit, which means that its integer value when cast to `signed char` will be negative. As a result, we can cast all characters to `signed char` and check whether any fall outside the standard ASCII encoding code point range \[0, 127]; if so, we replace such characters with `'?'`.



<h2 id="22-initial-pipeline">2.2 Initial Pipeline</h2>



Input is handled through a `user\_input` struct located in the `built\_ins` module.
The `user\_input` struct contains the following items:

1. `flag`
2. `unparsed\_string`
3. `tokens`
4. `invalid\_token`

### Processing and Tokenizing Input

When the user submits an input line by pressing `Enter`, the raw data is copied into the `unparsed\_string` member. Next, `GetParsedInput()` sanitizes this string by stripping all comments, replacing newline (`'\\n'`) characters with spaces, and trimming trailing whitespace to ensure clean string formatting for other functions. Once sanitized, the input string is split into individual tokens, which are sequentially appended to the `tokens` vector.

### Parsing Each Token

The `tokens` vector is then passed by reference to the `CommandCheck()` function. Here, a `std::string command` variable is initialized with the vector's first element. This is managed by `GetFirstToken()`, a utility function that extracts the front element from the `tokens` vector and immediately erases it, preparing the remaining vector elements for argument parsing. `CommandCheck()` then checks the `command` string and finds a matching operation for it. If found, it calls the matching function and passes the `tokens` vector by reference if the command accepts arguments.

### The `flag` and `error\_token`

The `flag` is an event flag (more on that in the next section).

The `error\_token` is needed for diagnostic purposes. If the event flag is set to `F\_INVALID\_INP`, `error\_token` will be assigned to whichever token raised this flag. After that, a specific error message function will be called with `error\_token` passed as an argument.

<h2 id="23-event-flags">2.3 Event Flags </h2>

An event flag is a `uint16\_t` integer variable for storing flag data. It is set to `0` by default. If any event occurs during data processing (e.g., a warning or an error must be raised), `flag` will be assigned the proper flag value. All flags are macros that start with the `F\_` prefix (for instance, the `F\_INVALID\_INP` error flag, which lives in the `built\_ins` section, or the `F\_TRUNCATION` warning flag, which lives in the `binary` section).

<h1 id="3-values">3. Values </h1>

Values are processed inside the converter engines. Before discussing the converters themselves, it is worth explaining how values are classified, in what objects they are stored, and how they are parsed.

<h2 id="31-value-types-mvalues-rvalues">3.1 Value Types (mValues \& rValues) </h2>

As of the current version of B1tLab, there are two types of values:

1. `mValues`
2. `rValues`

### `mValues`

`mValues` are mathematical values. These values can contain a sign as a separate character, a decimal point, and proper periodic notation. Their length is not fixed to any preset size. Leading zeros for the integer part as well as trailing zeros for the fractional part are omitted. `mValues` can be both decimal and binary. Here are examples:

```C++ 
    +5 or 5 
    5.34 
    5.12(122)
    -0
```

### Note on Signed Zero

`-0` is considered to be a valid `mValue`; however, its support is format-defined (some formats, like Sign-Magnitude, have a separate value reserved for negative zero, while others, like Two's Complement, do not).

### `rValues`

`rValues` are raw values. Their purpose is to show how any type of value actually resides in memory. Unlike `mValues`, they do not have a separate character for a sign (e.g., `+` or `-`), nor do they have a decimal point or periodic notation. Their size is fixed to a preset number of digits (depending on the format, presets may vary; for integer formats, `rValues` can be 8, 16, 32, or 64 digits in size). `rValues` can only be binary values, and the number of leading zeros matters. Here are examples:

```C++
00000000 
10101001
0110111011011101 
```

### Note

As mentioned earlier, the allowed bit size for `rValues` is format-defined. The same applies to their `mValue` representation. Different formats encode numbers differently, so depending on the specified format, a single `rValue` may or may not be represented as a single `mValue` (for instance, the `mValue` decimal representation of the `rValue` `10000000` in Sign-Magnitude is `-0`, but in Two's Complement it is `-128`).

<h2 id="32-value-parsing-validation">3.2 Value Parsing \& Validation</h2>

Each converter engine utilizes the `InpValueParser()` function to validate values passed as input. The parser lives in the `built\_ins` module. Its main purpose is to validate the value (checking whether the input contains any inappropriate digits, is formatted incorrectly, or exceeds designed limitations), process it according to the caller's arguments (removing leading/trailing zeros if needed), and return a parsed struct called `value`.

<h2 id="33-the-value-struct">3.3 The 'value' Struct</h2>

The `value` struct is one of the few structs used for storing number data (more on other structs later). It contains the following member variables:

1. `WHOLE\_return\_val`
2. `FRAC\_digit\_val`
3. `FRAC\_nonperiod\_val`
4. `FRAC\_period\_val`
5. `is\_a\_frac`
6. `sign`
7. `has\_explicit\_plus`
8. `flag`

The `value` struct splits the number into appropriate components that can be processed separately. After processing, the member string variables are overwritten, and a return value is constructed from them. Note that functions for internal processing (like utility converters) do not receive `value` as an argument or return it. Instead, a caller may pass a member variable of the struct so it can be processed.

### Components

`WHOLE\_return\_val`, `FRAC\_digit\_val`, `FRAC\_nonperiod\_val`, and `FRAC\_period\_val` are strings that store the integer part of a number, full fractional part, fractional non-periodic part, and periodic part, respectively.
Here are examples of what happens with these member variables after parsing:

```C++
Case No. 1: 5.21(34)
WHOLE\_return\_val = "5"
FRAC\_digit\_val = "21(34)"
FRAC\_nonperiod\_val = "21"
FRAC\_period\_val = "34"

Case No. 2: 5 
WHOLE\_return\_val = "5"
FRAC\_digit\_val = ""
FRAC\_nonperiod\_val = ""
FRAC\_period\_val = ""

Case No. 3: 5.
WHOLE\_return\_val = "5"
FRAC\_digit\_val = "0"
FRAC\_nonperiod\_val = "0"
FRAC\_period\_val = ""

Case No. 4: 5.34 
WHOLE\_return\_val = "5"
FRAC\_digit\_val = "34"
FRAC\_nonperiod\_val = "34"
FRAC\_period\_val = ""

Case No. 5: 5.(12)
WHOLE\_return\_val = "5"
FRAC\_digit\_val = "(12)"
FRAC\_nonperiod\_val = ""
FRAC\_period\_val = "12"

Case No. 6: .54 
WHOLE\_return\_val = "0"
FRAC\_digit\_val = "54"
FRAC\_nonperiod\_val = "54"
FRAC\_period\_val = ""
```

Note that if the user inputs `5` (Case No. 2), all fractional components of the number remain empty strings. However, when the user inputs `5.`, because of the decimal separator, we treat it as `5.0`. The same applies when the user inputs `.54`, which is treated as `0.54`.

### Attributes

`is\_a\_frac` and `sign` are boolean values. The former is intuitive, as it returns `true` if the number is fractional (note that `43.` and `5.0` are still treated as fractional values). The latter represents the sign of the value (more on that in the next section).

`has\_explicit\_plus` is a boolean variable used to help detect `rValue` inputs. If the input does not have an explicit sign character in front of it (e.g., `5`), `has\_explicit\_plus` is set to `false`; otherwise (e.g., `+5`), it is set to `true`.

The `flag` is an event flag (for more information, see Section [2.3 Event Flags](#23-event-flags)).

<h2 id="34-boolean-sign">3.4 Boolean Sign</h2>

All structs used to store or parse values have a `sign` member variable. `sign` is a boolean attribute intended to represent the sign/leading bit of the number. If `sign` is `0` (or `false`), the number is positive; otherwise, if `sign` is `1` (or `true`), the number is negative.



<h2 id="35-internal-structs">3.5 Internal Structs</h2>

It is worth noting that not all functions utilize the `value` struct to process data in non-trivial parsing cases. For some functions, a different representation of a number is much easier to work with. For instance, a rational representation of a number in the form $\\frac{a}{b}$ (where both numerator and denominator are integer values) is easier to handle than pure strings. For these reasons, there are two additional structs for internal usage:

1. `int\_parser`
2. `bin\_string\_parser`

### The `int\_parser` Struct

`int\_parser` is defined in the `built\_ins` section and consists of the following member variables:

1. `sign`
2. `numerator`
3. `denominator`
4. `flag`

We have already covered `flag` and `sign` (see Sections [2.3 Event Flags](#23-event-flags) and [3.4 Boolean Sign](#34-boolean-sign)).

Both `numerator` and `denominator` are `uint64\_t` integers that represent the value in rational form. To prevent division by zero, `denominator` is initialized to `1` inside the struct definition. Why represent the number in rational form in the first place? This choice was deliberate for two reasons:

1. It allows periodic fractions to be stored in exact, finite form.
2. Operations are performed directly on integers, meaning we avoid precision loss during arithmetic operations (which is why internal calculations never utilize floating-point values).

### The `bin\_string\_parser` Struct

`bin\_string\_parser` lives in the `binary` module, and its main purpose is to store and manipulate values for the Standard Conversion Engine. The struct consists of the following member variables:

1. `value`
2. `intermediate\_value`
3. `flag`

Note that there is no `sign` variable here. The sign of the variable (when dealing with `mValues`) can be directly encoded as a separate character into the `value` or `intermediate\_value` strings (though in most cases, this is unnecessary).

The `intermediate\_value` string is reserved for diagnostic purposes (for more information on intermediate values, see Section [4.2 The Standard Converter](#42-the-standard-converter)).

The `flag` variable is an event flag (for more details, see Section [2.3 Event Flags](#23-event-flags)).



<h1 id="4-conversion-engines">4. Conversion Engines </h1>



<h2 id="41-the-default-converter">4.1 The Default Converter  </h2>



The Default Converter converts a decimal `mValue` into a binary `mValue` (or vice versa). It is used to output exact, mathematically valid representations. The converter accepts only a single argument, which must be an `mValue`. If an `rValue` is passed to it instead, it will still be treated as a positive `mValue` (e.g., `10001001` will be treated as `+10001001`).

1. `DEC\_TO\_BIN()`
2. `BIN\_TO\_DEC()`

### Conversion Pipeline

The conversion pipeline for `DEC\_TO\_BIN()` and `BIN\_TO\_DEC()` engines is almost identical (with a few exceptions).

### Input Validation

Both `DEC\_TO\_BIN()` and `BIN\_TO\_DEC()` take a reference to the `tokens` vector as an argument via the `GetFirstToken()` function (more on tokens and initial input handling in Section [2.2 Initial Pipeline](#22-initial-pipeline)) and initialize the `input\_value` string with it. After that, the number is parsed in `InpValueParser()`, and the parsed result is stored in an instance of the `value` struct called `parsed\_input` (more on value parsing in Section [3.2 Value Parsing \& Validation](#32-value-parsing-validation)). The struct is then passed to `ErrorHandler()`, a utility function that raises a specific error and returns `true` if the input was invalid (terminating conversion). Only then is `return\_val` declared.

### Fractional Part Conversion

If the initial input was fractional, a specific code block is executed. Note that fractional conversion is handled differently across converter types.

### `DEC\_TO\_BIN()`

`parsed\_input.FRAC\_digit\_val` is converted into a rational fraction (via the `ToFraction()` utility function) and stored in an instance of `int\_parser` so it can be passed to `fracDecToBin()`. Finally, the return value of `fracDecToBin()` overwrites `parsed\_input.FRAC\_digit\_val` (since this function returns a string), and `return\_val` is appended with it.

### `BIN\_TO\_DEC()`

Instead of converting `parsed\_input.FRAC\_digit\_val` to a rational fraction altogether, the periodic and non-periodic digits are converted into decimal rational fractions separately via `fracBinToDecPeriod()` and `fracBinToDec()`, respectively. Both rational fractions are then added, and the result is stored in `int\_parser`. Finally, the rational fraction is converted into a decimal fraction via the `DecimalDivision()` utility function.

### Carry Logic

When the input's fractional part converges to one (e.g., $0.\\overline{9} = 1$), after fractional conversion is completed, `int\_parser`'s `numerator` will equal its `denominator`. In this case, the converter overwrites `parsed\_input.FRAC\_digit\_val` with `"0"` and increments `WHOLE\_return\_val` by one.

### Integer Part Conversion

`parsed\_input.WHOLE\_return\_val` is cast to an integer first and then passed to `intDecToBin()` as an argument. Finally, the return value of this function overwrites `parsed\_input.WHOLE\_return\_val`, which is appended to `return\_val`.

After all these steps, `return\_val` is assigned its proper sign, and `DEC\_TO\_BIN()` returns it.



<h2 id="42-the-standard-converter">4.2 The Standard Converter</h2>

The Standard Converter converts a decimal `mValue` into an `rValue` (or vice versa) according to a preset format. It is used to output hardware-level representations of numbers and handle edge cases. Unlike the Default Converter, where `DEC\_TO\_BIN()` and `BIN\_TO\_DEC()` are separate functions within a single engine, `STANDARD\_CONVERTER()` is a master function for both types of conversion. This design prevents code duplication, as the conversion type can be configured by passing the macros `DecToBin` and `BinToDec`.



<h3 id="421-modes">4.2.1 Modes</h3>

The Standard Converter engine supports four distinct modes. These modes make format configuration and value conversion more straightforward and convenient:

1. Value Mode
2. Set Format Mode
3. Show Format Mode
4. Set Default Mode

### Value Mode

Value Mode is the default operating mode of the Standard Converter, where the user can pass a number and an optional resize flag (more on resize flags below).

### Set Format Mode

In Set Format Mode, the user can change the preset format either for the current conversion type or across all conversion types.

To invoke Set Format Mode, the user must provide either the `--set\_local` flag or the `--set\_global` flag as the first argument (for more information on set flags, check the [5.3.2 Set Format Mode](#532-set-format-mode))

### Show Format Mode

In Show Format Mode, the user can view the currently preset format either for the current conversion type or across all conversion types.

To invoke Show Format Mode, the user must provide either the `--show\_local` flag or the `--show\_global` flag as the first argument (for more information on show flags, check the [5.3.3 Show Format Mode](#533-show-format-mode))

### Set Default Mode

Set Default Mode resets all previously configured formats to their default settings. Currently (since `STANDARD\_CONVERTER` supports integer formats only), the default format is Two's Complement for integers.

To invoke Set Default Mode, the user must provide the `--set\_default` flag as the first argument.



<h3 id="422-resize-flags">4.2.2 Resize Flags </h3>

Resize flags allow the user to alter the bit width of an `rValue`. Currently, there are four resize flags that work across all integer formats: `--resize\_to\_8`, `--resize\_to\_16`, `--resize\_to\_32`, and `--resize\_to\_64`.

#### Why Not Pad Flags?

During development, resize flags were initially named pad flags. However, I renamed them to resize flags because this definition is more accurate: resize flags do not simply insert leading zeros.

Consider resizing an 8-bit Sign-Magnitude representation of `-5` to 16 bits. The 8-bit representation is:

```c
10000101
```

Sign-Magnitude reserves the leading bit for the sign. If we simply added eight zeros to the front like this:

```c
0000000010000101
```

it would no longer represent `-5`. To prevent this, resize flags account for the sign representations of different formats, which naive padding does not do.



<h3 id="423-input-handling-pipeline">4.2.3 Input Handling Pipeline</h3>



`STANDARD\_CONVERTER()` takes a reference to the `tokens` vector as an argument via `GetFirstToken()` (more on tokens and initial input handling in Section [2.2 Initial Pipeline](#22-initial-pipeline)) and initializes `input\_token` with it. Then, `return\_val` is declared.

### Mode Selection

The `uint16\_t converter\_mode` variable is initialized with the return value of the `modeSelector()` utility function. Depending on this value, different modes are activated. If the mode requires a second argument, `GetFirstToken()` is called again. After that, `STANDARD\_CONVERTER()` terminates where applicable.

If the mode is Set Format Mode, depending on the flag and conversion type provided, the internal vectors `SDEC\_TO\_BIN\_formats` and `SBIN\_TO\_DEC\_formats` are overwritten with the format identifier provided as the second argument. Then `STANDARD\_CONVERTER()` terminates.

If the mode is Set Default Mode, these vectors are assigned the values from `default\_formats`. Then `STANDARD\_CONVERTER()` terminates.

If the mode is Show Format Mode, depending on the flag passed and converter type used, specific diagnostic functions are called to display current formats. Then `STANDARD\_CONVERTER()` terminates.

Finally, if the mode is Value Mode, `FORMAT\_CONVERTER()` is called. It parses the value, resolves resize flags, and calls a specific format converter function depending on the value type (whole or fractional) and preset format. `STANDARD\_CONVERTER()` then returns the converted value and terminates.



<h1 id="5-command-specifications">5. Command Specifications </h1>



<h2 id="51-utility-commands">5.1 Utility Commands  </h2>

### `exit`

`exit` sets the flag of the `user\_input` struct to `F\_EXIT`, which breaks the program loop and terminates execution.



<h2 id="52-default-conversion-commands">5.2 Default Conversion Commands  </h2>

Default conversion commands accept only a single argument. The argument must be a valid number, which is reinterpreted as an `mValue` (for details, see Section [4.1 The Default Converter](#41-the-default-converter)). After conversion, the resulting `mValue` is returned on a new line.

### `DEC\_TO\_BIN`

> DEC\_TO\_BIN \*\*decimal\_mVal\*\*

`DEC\_TO\_BIN` converts a decimal `mValue` and returns a binary `mValue`. Examples:

```c
DEC\_TO\_BIN -5.3
-101.0(1001)

DEC\_TO\_BIN 54 12 ;too many arguments
110110

The term '12' is not recognized as a valid command or a valid input.
```

### `BIN\_TO\_DEC`

> BIN\_TO\_DEC \*\*binary\_mVal\*\*

`BIN\_TO\_DEC` converts a binary `mValue` and returns a decimal `mValue`. Examples:

```c
BIN\_TO\_DEC 101
5

BIN\_TO\_DEC 2 ;'2' is not a binary digit
'2' isn't recognized as a valid input.

BIN\_TO\_DEC 11111111 ;an rValue treated as an mValue
255
```



<h2 id="53-standard-conversion-commands">5.3 Standard Conversion Commands</h2>

Standard conversion commands support three distinct modes and take up to two arguments depending on the selected mode.



<h3 id="531-value-mode">5.3.1 Value Mode </h3>

In Value Mode, the first argument must be a valid number, while the second argument can either be omitted or be a valid resize flag.

### Note on the 'u' Suffix

The `'u'` (or `'U'`) suffix attached to the end of an `mValue` or `rValue` implicitly changes the format of the current conversion type to Unsigned. The effect of this suffix is temporary and will not alter the preset format for subsequent conversions. Examples (assuming the preset format is Two's Complement):

```c
SDEC\_TO\_BIN 255u
11111111

SDEC\_TO\_BIN 255
0000000011111111

SBIN\_TO\_DEC 101
Intermediate value: 00000101
5

SBIN\_TO\_DEC 11111111u
255

SBIN\_TO\_DEC 11111111
-1
```

### `SDEC\_TO\_BIN`

> SDEC\_TO\_BIN \*\*decimal\_mValue\*\* \*\*\*--resize\_flag\*\*\* 

`SDEC\_TO\_BIN` takes a decimal `mValue` as an argument and outputs the converted `rValue` on a new line. Examples (assuming the preset format is Two's Complement):

```c
SDEC\_TO\_BIN 54
00110110

SDEC\_TO\_BIN -12 --resize\_to\_16
1111111111110100
```

### `SBIN\_TO\_DEC`

> SBIN\_TO\_DEC \*\*binary\_mValue\*\* \*\*\*--resize\_flag\*\*\*
> SBIN\_TO\_DEC \*\*rValue\*\* \*\*\*--resize\_flag\*\*\*

`SBIN\_TO\_DEC` takes a binary `mValue`, implicitly converts it to an `rValue`, and outputs the converted decimal `mValue` on a new line. If an `rValue` is provided initially, no implicit conversion occurs. Examples (assuming the preset format is Two's Complement):

```c
SBIN\_TO\_DEC 101
Intermediate value: 00000101
5

SBIN\_TO\_DEC -110 --resize\_to\_16
Intermediate value: 1111111111111010
-6

SBIN\_TO\_DEC 10110111 ;rValue provided
-73

SBIN\_TO\_DEC 10110111 --resize\_to\_16
Intermediate value: 1111111110110111
-73
```



<h3 id="532-set-format-mode">5.3.2 Set Format Mode</h3>



> SDEC\_TO\_BIN \*\*\*--set\_flag\*\*\* \*\*FORMAT\*\*
> SBIN\_TO\_DEC \*\*\*--set\_flag\*\*\* \*\*FORMAT\*\*

In Set Format Mode, the Standard Converter expects the first argument to be a set flag (`--set\_local` or `--set\_global`). The second argument must be a valid identifier for the desired format. `--set\_local` changes the format for the current converter type, whereas `--set\_global` alters the format across both converter types. Examples:

```
SDEC\_TO\_BIN --set\_local ONES\_COMP
Format 'ONES\_COMP' has been set for 'SDEC\_TO\_BIN' integer conversions.

SBIN\_TO\_DEC --set\_global SIGN\_MAG
Format 'SIGN\_MAG' has been set for 'SDEC\_TO\_BIN' integer conversions.
Format 'SIGN\_MAG' has been set for 'SBIN\_TO\_DEC' integer conversions.
```



<h3 id="533-show-format-mode">5.3.3 Show Format Mode</h3>



> SDEC\_TO\_BIN \*\*\*--show\_flag\*\*\*
> SBIN\_TO\_DEC \*\*\*--show\_flag\*\*\*

In Show Format Mode, the Standard Converter expects a single argument, which must be a show flag (`--show\_local` or `--show\_global`). `--show\_local` displays the preset format for the current converter type, whereas `--show\_global` displays preset formats across both converter types. Examples:

```
SDEC\_TO\_BIN --show\_local

SDEC\_TO\_BIN:
INT  : TWOS\_COMP
FRAC :

SBIN\_TO\_DEC --show\_global

SDEC\_TO\_BIN:
INT  : TWOS\_COMP
FRAC :

SBIN\_TO\_DEC:
INT  : TWOS\_COMP
FRAC :
```



<h3 id="534-set-default-mode">5.3.4 Set Default Mode</h3>



In Set Default Mode, the Standard Converter expects a single argument: `--set\_default`. The `--set\_default` flag resets preset formats to default settings across all converter types. Example:

```
SDEC\_TO\_BIN --set\_default
Default formats have been set for 'SDEC\_TO\_BIN' and 'SBIN\_TO\_DEC':
INT  : TWOS\_COMP
FRAC :
```

<h1 id="6-diagnostic-message-specifications">6. Diagnostic Message Specifications</h1>



<h2 id="61-built\_ins">6.1 built\_ins</h2>

### ConversionError()

> '\*\*argument\*\*' isn't recognized as a valid input.

An error indicating that the argument passed to the converter engine is invalid. Examples:

```C++
SDEC\_TO\_BIN ff              ;ff is not a number
DEC\_TO\_BIN 54u              ;DEC\_TO\_BIN doesn't support the u postfix
SDEC\_TO\_BIN --set\_global 54 ;--set\_global expects a format 
```

### InvalidCommandError()

> The term '\*\*token\*\*' is not recognized as a valid command or a valid input.

An error raised when an invalid or unsupported command is entered. Examples:

```C++
empty        ;empty is not a command
--set\_global ;flag can't be a command!
54           ;value is still not a command
```

### OverflowError()

> Input is out of bounds (convertible value should be greater than or equal to \*\*lower\_bound\*\* or less than or equal to \*\*upper\_bound\*\*)

An error indicating that the integer part of the number is out of the designated range. Examples:

```C++
DEC\_TO\_BIN 18446744073709551616     ;the number is too large
DEC\_TO\_BIN -9999999999999999999999  ;the number is too small 
```

### MantissaOverflowError()

> Input has too many digits in the fractional part (max convertible amount is \*\*bound\*\* digits).

An error indicating that the number of digits in the fractional part exceeds designated limits. Example:

```C++
DEC\_TO\_BIN 3.141592653589793238462643383279502884 ;too many digits in the fractional part
```



<h2 id="62-binary">6.2 binary</h2>



### IntermediateValue()

> Intermediate value: \*\*value\*\*

A diagnostic message used in the Standard Converter to display the intermediate representation of a number during conversion. This message is displayed if the value was modified during processing. Examples:

```C++
SBIN\_TO\_DEC 101              ;will be converted to an rValue
Intermediate value: 00000101
```

### WrapAroundWarning()

> The value has been wrapped around within \*\*N\*\* bits.

A warning used in the Standard Converter to indicate that wrap-around within a given bit width occurred. Examples:

```C++
SDEC\_TO\_BIN -5u
The value has been wrapped around within 8 bits.

SBIN\_TO\_DEC -101u --resize\_to\_16 
The value has been wrapped around within 16 bits.
```

### SetFormatMessage()

> Format '\*\*format\*\*' has been set for '\*\*conversion\_mode\*\*' \*\*value\_type\*\* conversions. 

A diagnostic message used in the Standard Converter to indicate that a specific format has been assigned for the specified conversion type (`DecToBin` or `BinToDec`) and value type (integer or fractional). `SetFormatMessage()` is triggered by set flags (`--set\_local` or `--set\_global`). Examples:

```
SDEC\_TO\_BIN --set\_local ONES\_COMP ;One's Complement is compatible with integers only
Format 'ONES\_COMP' has been set for 'SDEC\_TO\_BIN' integer conversions.

SBIN\_TO\_DEC --set\_global SIGN\_MAG
Format 'SIGN\_MAG' has been set for 'SDEC\_TO\_BIN' integer conversions.
Format 'SIGN\_MAG' has been set for 'SBIN\_TO\_DEC' integer conversions.
```

### SetDefaultMessage()

> Default formats have been set for 'SDEC\_TO\_BIN' and 'SBIN\_TO\_DEC':
INT  : \*\*default\_format\*\* 
FRAC : \*\*default\_format\*\*

A diagnostic message used in the Standard Converter to indicate that the engine has been reset to default formats across both conversion types. `SetDefaultMessage()` is triggered by the `--set\_default` flag. Example:

```
SDEC\_TO\_BIN --set\_default
Default formats have been set for 'SDEC\_TO\_BIN' and 'SBIN\_TO\_DEC':
INT  : TWOS\_COMP
FRAC :
```

### ShowLocal()

> \*\*conversion\_mode\*\*:
INT  : \*\*preset\_format\*\*
FRAC :

A diagnostic message used in the Standard Converter to show preset formats for integer and fractional values in the specified conversion type (`SDEC\_TO\_BIN` or `SBIN\_TO\_DEC`). It can also display formats across all conversion modes when invoked with show flags (`--show\_local` or `--show\_global`). Examples:

```
SDEC\_TO\_BIN --show\_local

SDEC\_TO\_BIN:
INT  : ONES\_COMP
FRAC :

SBIN\_TO\_DEC --show\_global

SDEC\_TO\_BIN:
INT  : ONES\_COMP
FRAC :

SBIN\_TO\_DEC:
INT  : TWOS\_COMP
FRAC :
```

