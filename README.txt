This is my implementation of The World's Hardest Game! I played a ton of this back in the day, so it's pretty fun implementing it 
on my own.

In here, the player(the red square) will try to maneuver through a maze of moving enemies(the blue squares). These enemies move at
random speed horizontally, and if they colides with the red square, the player will lose and have to start over at the beginning.
If they die, they have to press BUTTON_B to start over, and the death count will be incremented for every death.

To control the red square, the player has to use BUTTON_RIGHT, BUTTON_LEFT, BUTTON_UP, BUTTON_DOWN to move the square around the screen.
This red square, unlike the enemies, can freely move in a 2-dimensional fashion.

To restart at anytime during the game, the player can press BUTTON_A to reset to the starting screen. This will reset the death count
of the player up until that point.

The goal is to reach the green line! Pretty simple! Afterward, the player only needs to control the red square toward the trophy to 
get the winning screen!
