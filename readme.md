# Platformer Joystick
## TODO

- Update the README
- Change the default settings
- Add a level arg to change default setting

### Notes

- Left and right control player 1's movement.
- Up and down control player 2's movement.
- Levels can set whether or not they have joysticks by default by adding a text object with "--joystick-default".
- Item IDs can be used for various properties of the joystick.
  - Enable by adding a text object with "--joystick-counters".
  - Item ID 3740 will be set to 1 if joystick is enabled.
  - Item ID 3741 will be set to -1, 0, or 1, depending if the joystick is on the left, middle, or right.
  - Item ID 3742 will be set to -1, 0, or 1, depending if the joystick is on the bottom, middle, or top.