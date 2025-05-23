# Platformer Joystick
## TODO

- Update the README
- Change the default settings
- Add a level arg to change default setting

### Notes
Joystick input is managed with player 1 and player 2 controls.
- Left and right control player 1
- Up and down control player 2

Levels can enable joysticks by default by including a text object that says `--joystick-enabled`. I recommend setting it to hidden.

The mod can manage item ids if the level includes a text object that says `--joystick-counters`. I recommend setting it to hidden.
- Item Id 3740: 1 if counters are enabled and the mod is enabled.
- Item Id 3741: Left and right for the joystick
- Item Id 3742: Up and down for the joystick