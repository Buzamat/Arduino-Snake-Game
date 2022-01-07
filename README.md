Snake game.

Components used:

Arduino UNO
Max7219 Driver
LCD display
Buzzer
Joystick
2x LED matrix 8x8 (one can be removed, never ended up using it)
Matrix shield
Breadboard & wires
Resistor of 220 and 100k Ω
Electrolytic capacitor of 10 µF and of 104 pF

The project is split into two main states, when the game is running and when the menu is on.

The menu:

When you power up the arduino, the lcd display will present the main menu. Here you have the option to start the game, change your settings or view your highscores. There is also another hidden element in the main menu, the "set score" ui
 but the player cannot access it directly, to do that he has to start and finish a game. In the settings menu you can set the contrast of the lcd or reset the highscores. In the highscore menu you can view each one of the 3 highscores.
We save the contrast settings and the highscores in EEPROM so we can keep them between game sessions. 
Each menu item has a bool that checks if it's active or not, and the whole menu system is coded so it cascades like a tree starting from the main menu and going down.

Main menu:
- Start new game
- Settings

    o Set contrast
  
    > Set contrast ui
 
     o Reset highscores
  
    > Confirmation ui
 
- Highscore

  o Display highscores ui
- Set Score (access when you finish the game)

    o Display score (If it's not a highscore, you are sent back to main menu, if it's a new highscore you are sent to the set highscore ui)
    
      > Set highscore ui (when you get a highscore, then you are sent back to main menu)


The actual game:

When the player presses the start new game button, the actual Snake game starts. This is just a simple snake game with a small catch, the food dissapears after a certain amount of time, and the faster you eat it, the more points you get. Also, with 
time, the speed of the snake will increase (up to a certain point, otherwise it would become unplayable).

When you begin the game, the code runs a subprogram that resets all the game values like snake lenght, score and speed, then it chooses a random seed for the food spawning algorithm so we don't get identical food locations between games.
The food spawning algorithm checks if the location of the next food isn't inside the snake body before spawning it. As you eat the food, the score updates on the lcd display and the buzzer sends a sound effect.
The food is also constantly blinking so we can know the difference between food and snake.
We use a variable to check when we updated something on the board so we know when to update the matrix display, this way we avoid screen flickering. Also, because of the way the snake game works, we only move a couple of pixels with each move,
mainly the food, the end of the tail and the head of the snake, the rest of the body remains the same. 
The snake body is saved up in a list where we save the coordinates of each bodypart of the snake, first element on the list being the head and the last element being the end of the tail, each new bodypart is added to the end of the list mainly by shifting every bodypart one position behind
and giving the "head" the coordinates of the food we just ate.
We use a variable representing direction to know where the snake is going, we change that using the joystick and each move cycle we check what is in front of the snake (using the direction we move in as a parameter). If there is food, we grow the snake, raise the score, spawn new food.
If there is an empty space, we move the snake forward.If it goes offscreen we loop it from the opposite wall. If in front of it we find another part of the snake, the game is over, we play an animation where the snake disolves from it's tail to it's head, then we send the player back in the main menu.

[Youtube presentation](https://www.youtube.com/watch?v=Bbhn56sG0fI)
