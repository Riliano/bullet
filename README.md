# bullet
Displays a bullet pattern

Part of Daffodil Yellow project

This is used to preview shmup(bullet hell) style patterns

TODO:
Add file as a valid input
Replace the joke drawing function with and actual one
More testing
Finalize on what the syntax is

Right now the syntax hasn't been finalized and is subject to change
The indents are purely for readabilty

s 220 240 180 5 200 <- create a (s)pawner with coordinates 220,240, with initial angle of 180 degress, it will spawn 200 bullets in interval of 0.5 s between them
	p r -3 30 <- part of the (p)attern of the spawner(since it's after the spawner declaration but before the bullets it will (r)otate with -3 degrees every 3.0 seconds
	b 0 0 80 <- a (b)llet it will start with 0,0 offset from the spawnet and it will last on screen 8.0 seconds
    p l 7 0 200 <- part of the (p)attern of the bullet(since its under the bullet) it will move with speed of 7 in a direction of 0 degree for 20.0 seconds
