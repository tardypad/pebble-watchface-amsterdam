Amsterdam Pebble Watchface
==========================

[Pebble](https://www.pebble.com/) watchface displaying the Amsterdam flag

Designed for platforms:
- Basalt (Time, Time Steel)
- Chalk (Time Round)

Built with SDK 4

![Chalk roll animation](/assets/chalk/animation_roll.gif?raw=true "Roll animation")
![Basalt slide animation](/assets/basalt/animation_slide.gif?raw=true "Slide animation")

Features
--------

- display random XXX animations every minute
- display date
- local settings for display
- handle Timeline Quick View

Build
-----

To generate the animations, 1 extra tool, outside Pebble's SDK, is needed:
- [svg2pdc](https://github.com/tardypad/pebble-svg2pdc/blob/master/tools/svg2pdc.py)

Special options:
- _--debug-logs_: Build with debugging logs
- _--debug-slow_: Build with slow animations

Example usage: _pebble build -- --debug-logs --debug-slow_

Licenses
--------

Source code is released under the MIT license (See the [LICENSE](LICENSE) file)

Application specific graphics are released under Creative Commons CC BY


Enjoy!