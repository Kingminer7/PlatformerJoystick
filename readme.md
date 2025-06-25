# Platformer Joystick

Adds a way for level creators to have joystick controls.

## How it works
Levels can enable joysticks by including a text object that says `--enable-joystick`. I recommend setting it to hidden.
- Note: iOS users may need to disable smart punctuation, since trying to type `--` ends up deleting the 2 dashes because iOS attempts to replace two dashes with an em dash.

When joysticks are enabled, you can use the following ids for detection
- Item id 3740: 1 if the mod is enabled and the previously mentioned text object is in the level.
- Item id 3741: Left and right for the joystick
- Item id 3742: Up and down for the joystick
