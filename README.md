# Console Game of life
![Alt Text](https://raw.githubusercontent.com/tim-braslavsky/GOL/main/LIFE.gif)

This is a project based off One Lone Coder's [tutorial](https://www.youtube.com/watch?v=E7CxMHsYzSs).
This is an implimentation of [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in the command console. I added a way to chose the color that is used for seeding as well as inheritence and a selection bias selction for color of new life, as a way to psuedo model the process of natural selection. A couple notes there is a 1% chance for a mutation of the color and all color are codiminant. If random it chosen for the color it is a random seed color per placement and random for the fittest color being random changes every 150 frames.

## Color co-dominance:
- RED + RED = RED
- BLUE + BLUE = BLUE
- GREEN + GREEN = GREEN
- RED + BLUE = MAGENTA
- RED + GREEN = YELLOW
- BLUE + GREEN = CYAN

One gene is ingerited from each parent with the appropriate chances(punnit square).


*Feel free to download, play and modify as you see fit. This was a project for learning and fun only.*

============================================================

##EXE DOWNLOAD##
If you are looking for just the exe you can download it [here](https://github.com/tim-braslavsky/GOL/blob/main/celularAutamata.exe)
