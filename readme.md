# Platformer Joystick

Adds a way for level creators to have joystick controls.

## How it works
Joystick input is managed with player 1 and player 2 controls.
- Left and right control player 1
- Up and down control player 2

Levels can enable joysticks by including a text object that says `--enable-joystick`. I recommend setting it to hidden.

The mod can manage item ids if the level includes a text object that says `--joystick-counters`. I recommend setting it to hidden.
- Item id 3740: 1 if counters are enabled and the mod is enabled.
- Item id 3741: Left and right for the joystick
- Item id 3742: Up and down for the joystick
