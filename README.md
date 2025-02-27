# Frame1/B0XX layout style open-source digital controller software for the Raspberry Pi Pico (v0.3)

This is a modular and easily extensible digital controller software for the Raspberry Pi Pico, that can identify as various controllers and communicate over the Joybus (Gamecube/Wii) and USB protocols, with several digital to controller representation conversion modes built-in: Melee, Ultimate, generic controller and generic keyboard.

#### Supported controller representations:
- Gamecube controller (Joybus)

- Gamecube controller to USB Adapter aka WUP-028 (USB, Switch compatible, compatible with WinUSB-only features)

- Wired Fight Pad Pro (USB, Switch compatible)

- 8KRO Keyboard (USB)

All controller representations are plug-and-play, including the WUP-028 one, that will install WinUSB on plug-in.

Modes combining (GPIO -> Button sets) (Button sets -> Controller representation) (Controller representation -> USB Configuration/Joybus) configurations can be chosen at runtime initialization by pressing a given key when powering the controller.

This project doesn't utilize TinyUSB, but instead implements the USB protocol itself to manage runtime-dependant descriptors. This is largely based on the dev_lowlevel pico-example project, although fully migrated to C++, with some extensions such as WCID compatibility and multi-packet control transfers.

### LEGAL INFORMATION

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

In particular, when communicating over USB, device using this software may use the 0x057E USB Vendor ID, that is affiliated with Nintendo, and other proprietary USB vendor IDs. By uploading this software onto your board, you assert that you understand what that means and take entire responsability for it.

### Safety information

Don't have this board plugged via USB and via its Gamecube port at the same time. This would feed the USB 5v to the 3v line of the console and likely damage it.

If you want to prevent this electrically, use Schottky diodes, or power VSYS with the 5v from the console and don't connect the console 3v. Be aware that doing this implies the controller won't work on consoles with broken rumble lines anymore.

### Perks over atmega32u4 based controllers:

- Up to 25 inputs + a console data line

- No 5V input required (Voltage regulator accepts 3.3V)

- No logic level shifter required (3.3V GPIO by default)

- Extremely simple programming (USB drag and drop)

- Much higher (~8x) MCU speed allows reacting to console poll rather than having to predict them, hence better latency (i.e matches Frame1 latency give or take a few cycles which is slightly better than that of B0XX/other atmega32u4 based controllers)

- 1000Hz effective reporting over USB, which some atmega32u4 based controllers (notably the B0XX) don't have, granting better latency & latency stability

- Identifying as a GCC to USB adapter where your controller is plugged in port 1 means that this is compatible with WinUSB-only features (Kristal's netcode, timing dispersion reduction)

- Very cheap ($4) and widely available

### Modes

As of this release, 6 modes are built-in.

- Not plugged into USB => Console mode (Melee F1 DAC algorithm + Joybus). If you're not plugged into USB, you enter this mode.

- Plugged into USB, nothing pressed => Melee GCC to USB adapter mode (Melee F1 DAC algorithm + Adapter USB configuration).

- GP6 (by default, MX) => Ultimate GCC to USB adapter mode (Ultimate DAC algorithm + Adapter USB configuration).

- GP5 (by default, L) => Wired Fight Pad Pro with Melee logic (Melee F1 DAC algorithm + Wired Fight Pad Pro USB configuration).

- GP4 (by default, Left) => Wired Fight Pad Pro with dedicated logic (Wired Fight Pad Pro DAC algorithm + Wired Fight Pad Pro USB configuration). See lower for DAC algorithm explanation.

- GP0 (by default, Start) => 8KRO Keyboard (8 Keys set DAC algorithm + 8KRO Keyboard USB configuration). See lower for mapping.

### Software architecture

The code is split in 4 parts.

- GPIO to button sets: define pinouts. By default only one is defined, the one advised lower in this README. Change it directly if you use a different pinout for whatever reason, change it. (If you don't plan on making other changes, using the default allows you to use the .uf2 directly instead of recompiling on every update) If, say, you want to have the same controller logic (SOCD handling, etc.) for 2 characters but have a different button mapping, make 2 profiles there.

- DAC algorithms aka, from button sets to a controller representation. Controller representation being, as of this release, a Gamecube controller, a Wired Fight Pad Pro or an 8KRO keyboard. This is where the *digital to analog* and in fact, digital to anything logic, happens. This is where the PM/P+/Ultimate logic will be when I get to them.

- USB configurations: this is where the structure that the DAC algorithms must fill is exposed, and where the logic specific to identifying as a given USB peripheral happens. As of this release, this includes a WUP-028 adapter, a Wired Fight Pad Pro and an 8KRO Keyboard.

- Communication protocols: This is the protocol specific code. You most likely don't need to touch these. 

### Melee mode notes

The logic is that of the Frame1, with the slight side B and parasol dashing nerfs removed. They can be added back in the code via a toggle.

### Wired Fight Pad Pro mode logic

With the Melee F1 DAC algorithm, Start is mapped to Home. L, R and Z are respectively mapped to ZL, ZR and R. -, +, L and Photo are inaccessible. The purpose of this mode is to allow playing on a PC setup with other people that are using vanilla Gamecube controllers through an adapter, as Slippi can't handle multiple adapters at once. There may be slight analog discrepancies occuring as a result of using the Standard Controller mode (max 1 tick).

In dedicated mode, Modifiers and LS/MS are repurposed. This means you can only access cardinals and diagonals on the control stick.
- LS => L
- Z => R
- L => ZL
- R => ZR
- MX => -
- MY => +
- Start => Home
- MS and Left => Dpad left
- MS and Right => Dpad right
- MS and Up => Dpad up
- MS and Down => Dpad down
- MS and Start => Photo

### 8KRO Keyboard mappings

Image TODO

### Adapter mode information

In adapter mode, upon connecting the Raspberry Pi Pico to your PC after uploading the software at hand onto it, the Raspberry Pi Pico will identify as a "WUP-028" i.e a Gamecube controller to USB adapter. Things will behave as if the Gamecube controller this board emulates was plugged in port 1. You need to use the "GameCube Adapter for Wii U" in the Slippi Controllers tab (which is the default). You don't need to configure anything.

However, in order to use this controller, you must first unplug any Gamecube controller to USB adapter connected to your PC, as softwares will typically only support communicating with one adapter at a time (notably Dolphin/Slippi).

Note that this mode of operation requires the "WinUSB" driver to be associated with this device. If you've used a Gamecube controller to USB adapter before, chances are you've associated WinUSB to the WUP-028 through Zadig. But even if you haven't, you shouldn't need to, as WinUSB will be installed automatically upon plugging the Pico board in.

Note that polling rate enforcements using the HIDUSBF filter driver that apply to "standard" WUP-028s will apply to this board, so enforced rates less than 1000Hz you could have configured would decrease this controller's performance, which is 1000Hz by default.

Automated WinUSB installation is very experimental. If you encounter any driver related issue, please contact me.

### How to program your board:

- Download the latest release (on the right of the Github page)

- Plug in your Raspberry Pico to your computer via USB while holding the "BOOTSEL" white button on the board.

- The board should appear as an external drive. Put the .uf2 in there. The board should disconnect and be ready for use.

### How to wire the board:

Provided you want to use the Frame1/B0XX like logic, I advise you to wire the board following "How to wire the board" section specifications. Otherwise, you will need to compile the project yourself after changing the pinout in `main.cpp`, and do it again every new release. If you compile the source code anyway for whatever reason, clone the .git repository (`git clone pathToTheDotGitCopiedFromTheGreenTopRightButtonOfThisPage` in Git Bash) instead of only downloading a .zip.

If you download the zip, it will still work, because I gave up on using submodules since nobody read this.

Still, you should clone the repository if you intend to modify anything, or you'll have to do it later to pull from upstream.

![image](https://i.imgur.com/75a7UYr.png)

Switches/buttons will have two pins. Connect one of them to Ground (daisy chaining advised) and the other to a Pico GPIO pin as per the following mapping:
- Start to GP0 (pin 0)
- Right to GP2 (pin 4)
- Down to GP3 (pin 5)
- Left to GP4 (pin 6)
- L to GP5 (pin 7)
- MX to GP6 (pin 9)
- MY to GP7 (pin 10)
- CStick Up to GP12 (pin 16)
- CStick Left to GP13 (pin 17)
- A to GP14 (pin 19)
- CStick Down to GP15 (pin 20)
- CStick Right to GP16 (pin 21)
- Up to GP17 (pin 22)
- MS to GP18 (pin 24)
- Z to GP19 (pin 25)
- LS to GP20 (pin 26)
- X to GP21 (pin 27)
- Y to GP22 (pin 29)
- B to GP26 (pin 31)
- R to GP27 (pin 32)
- Ground to pins 33 and 38 (i.e connect pin 33 to 38 together, then connect them to the console grounds and to your daisy chains of switch ground pins)
- Gamecube data line to GP 28 (pin 34)
- 3.3V to VSYS (pin 39)
- Don't connect your 5V input

### Troubleshooting

Console mode/B/R don't work -> Did you connect pin 33 to ground ?

Nothing but B/R works -> Did you connect pin 38 to ground ?

### Closing notes

I intend to add P+/PM support shortly. The v1.0 release will happen after that. Until then, the project is in beta and default configurations (mode pins, keyboard mappings, wired fight pad pro mappings) may change. Before I can do the full release, everything has to be tested thoroughly, so any help with testing is appreciated. Especially the Ultimate mode !

### Contact

Discord: Arte#9281

Twitter: https://twitter.com/SSBM_Arte
