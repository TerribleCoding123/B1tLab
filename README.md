![](https://www.dropbox.com/scl/fi/0518gpviina6h946vjjsx/B1tLab-logo.png?rlkey=sg2pcvvwykmrpprtppnc3as0d&st=zf9jj4nk&raw=1)
**B1tLab is a CLI tool for converting integer representations, exploring binary formats, and generating accurate binary equivalents of decimal values.**


## **📌 Table of Contents**

### &nbsp;&nbsp;&nbsp;&nbsp;• [🔍Overview](#overview)
### &nbsp;&nbsp;&nbsp;&nbsp;• [✨Features](#features)
### &nbsp;&nbsp;&nbsp;&nbsp;• [🚀Usage](#usage)
### &nbsp;&nbsp;&nbsp;&nbsp;• [📦Installation](#installation) 
### &nbsp;&nbsp;&nbsp;&nbsp;• [🏗️Architecture](#architecture)
### &nbsp;&nbsp;&nbsp;&nbsp;• [🗺️Roadmap](#roadmap)
### &nbsp;&nbsp;&nbsp;&nbsp;• [⚙️Tech Stack ](#tech-stack)
### &nbsp;&nbsp;&nbsp;&nbsp;• [📄License](#license)
### &nbsp;&nbsp;&nbsp;&nbsp;• [✍️Author's Note](#note)


<h2 id="overview">🔍Overview</h2>

**B1tLab** is an interactive conversion engine designed to explore both exact numerical representations and low-level hardware formats at the bit level. Built for developers, students, and low-level computing enthusiasts, the tool seamlessly handles standard mathematical conversions—including signed values, ordinary fractions, and repeating periodic ones—while offering dedicated modes to inspect how these values map onto various formats. By bridging abstract math with machine-level constraints, B1tLab allows users to observe exact fractional behavior alongside hardware edge cases like truncation, signed zero, and integer overflow wrap-around.

<h2 id="features">✨Features</h2>

#### ⚡ Core Conversion Features

-   ✅ **Generous Limits:** Handles arbitrary integers up to $2^{64} - 1$ and up to 16-digit fractional mantissas.
    
-   ✅ **Periodic & Decimal Fractions:** Full support for processing and converting ordinary and repeating periodic fractions.
    
-   ✅ **4 Standard Integer Formats:** Native support for **Unsigned**, **Sign-Magnitude**, **One's Complement**, and **Two's Complement**.
    
-   ✅ **Flexible Bit-Widths:** Supports variable-size integers and fixed-size presets (8, 16, 32, and 64-bit), with dynamic resizing via `--resize_to_N` flags.
    
-   ✅ **Hardware Edge Cases:** Handles truncation, wrap-around, and signed zero representations in different formats (`-0`).
    
-   ✅ **Configurable Scopes:** Toggle integer encoding formats locally or globally across converters using `--set_local` and `--set_global`.
    

#### 🖥️ Interactive Shell & UI

-   ✅ **Dynamic Syntax Highlighting:** Real-time color feedback as you type commands, flags, and literals.
    
-   ✅ **Rich Output Formatting:** Colorized return values, formatted flags, and italicized diagnostic messages for intermediate states.
    
-   ✅ **Audio Alerts:** Built-in system bell chime (`\a`) triggered on syntax errors and invalid inputs.
    
-   ✅ **Assembly-Style Comments:** Supports inline and standalone comments starting with `;`.
<h2 id="usage">🚀Usage</h2>
If you want to terminate the program, just type `exit` and hit Enter.

Try typing `DEC_TO_BIN` and passing a value in order to convert it into a properly-notated binary representation. Here are the examples:
![enter image description here](https://www.dropbox.com/scl/fi/hqhnnv5gv61x3s6r2seig/Example_1.png?rlkey=rmj53hvfeklehvacqjei6e1cb&st=5kj4mcl2&raw=1)

You can do the same with `BIN_TO_DEC` (***Note that `BIN_TO_DEC` accepts only binary values***): 
![enter image description here](https://www.dropbox.com/scl/fi/encvg07v85kvfgqpo8g1a/Example_2.png?rlkey=1wgezeh5ohhn3ut31epxzrgj4&st=eis1dbgb&raw=1)


Now, in order to see how raw binary values are actually stored in registers, type `SDEC_TO_BIN` and pass a value to it:
![enter image description here](https://www.dropbox.com/scl/fi/t4udgn0xdfu20ds35ktbx/Example_3.png?rlkey=ls6lbn1p73c02jt4sqau2g4eb&st=cnx0o6sn&raw=1)

Same works for `SBIN_TO_DEC`:
![enter image description here](https://www.dropbox.com/scl/fi/8qrlgxx0epvx0jp6a9dkz/Example_4.png?rlkey=qrj2eckhp4vrf9hlbrldyb9kd&st=6bze4ug4&raw=1)

`SDEC_TO_BIN` and `SBIN_TO_DEC` also allow to pass special flags to them:
![enter image description here](https://www.dropbox.com/scl/fi/3pu4z6c0hdwrazw06ds4b/Example_5.png?rlkey=c59am6qompdstestnd3vvt5vh&st=of9p07kp&raw=1)

For a full breakdown of commands, flags, value types, hardware edge cases, and how everything works under the hood, check out the **[🏗️ Architecture](#architecture)** section.

<h2 id="installation">📦Installation</h2>

### Prerequisites

Before building BitLab, make sure you have the following installed:

* **C++20 Compiler** (such as `g++`, `clang++`, or MSVC)
* **CMake** (v3.31 or higher)
* **Git** (optional, for cloning)



### Building from Source

#### 1. Get the source code
Clone this repository to your local machine, or click **Code > Download ZIP** at the top of this GitHub page and extract the archive.

#### 2. Navigate to the project directory
Open your terminal or command prompt and change into the project folder:

```bash
cd BitLab
```

#### 3. Configure the build
Generate the build system files cleanly inside a dedicated `build/` directory:
```bash
cmake -B build
```
#### 4. Compile the binary
Compile the source code using CMake:
```bash
cmake --build build
```
#### 5. Run B1tLab
Locate the `B1tLab.exe` file and run it.



<h2 id="architecture">🏗️Architecture</h2>

For a detailed guide on how and why some components of BitLab work this way, check out [`ARCHITECTURE.md`](ARCHITECTURE.md).

<h2 id="roadmap">🗺️Roadmap</h2>

 Here is the list of major goals for future versions:
 
### 🛠️Architecture & CLI
🔘 **Optimization & Architectural Improvements:** Rewrite input handling logic, implement reference-based string functions (`std::string_view` / pass-by-reference), and improve overall code structure.
🔘 **New Commands & Features:** Introduce new helper commands, interactive menus, and detailed diagnostic error messages.

### 🔬 Number Representations & New Formats
🔘 **Scientific Values (`sValues`):** Implement parsing support for scientific notation formats containing a mantissa and exponent (e.g., `4.235e10`).
🔘 **10 Floating-Point Binary Formats:** Add support for key FP binary formats including IEEE 754 standards, bfloat16, TensorFloat-32 (TF32), etc.
🔘 **5 Floating-Point Decimal Formats:** Implement most common FP decimal formats.
🔘 **5 Fixed-Point Formats:** Implement configurable fixed-point representations.
 
<h2 id="tech-stack">⚙️Tech Stack</h2>

* **Language:** C++20
* **Build System:** CMake 3.31
* **Libraries:** [`replxx`](https://github.com/AmokHuginnsson/replxx) *(for dynamic syntax highlighting and input filtering)*
* **Terminal:** ANSI escape code supported terminal with 24-bit True Color and rich text formatting
* **Target Platforms:** Cross-Platform (Windows, Linux, macOS)

<h2 id="license">📄License</h2>

This project is licensed under the MIT License — see the [`LICENSE.txt`](LICENSE.txt) file for details.


<h2 id="note">✍️Author's Note</h2>
Thank you for reviewing my project and taking the time to check it out—it honestly means a lot to me! This is the first serious software project I have published on GitHub, and I have already learned an incredible amount from building it.

As a student, I know there is still plenty to refine. I would deeply appreciate any feedback, constructive critique, or suggestions regarding architectural quirks, bugs, or bad practices you might find (I'm sure there are a few!).

Thanks again for taking the time to explore B1tLab!

