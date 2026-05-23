# Platformer Joystick

Adds a way for level creators to add joystick controls.

### Warning for keybinds users:

- The defaults may conflict with your jump keybind, make sure to have a jump keybind other than W when playing joystick based levels!

## Usage

Players can enable a setting to show the joystick in all platformer levels regardless of if the level supports it or not.

Creators can enable joysticks in a menu in the level settings popup in the editor. 

When joysticks are enabled in a level, you can use the following ids for detection

- Item id 3740: 1 if the mod is enabled.
- Item id 3741: Left and right for the joystick
- Item id 3742: Up and down for the joystick

The joystick also moves player 1 left and right.