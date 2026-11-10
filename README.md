# Qt-flap
**Flappy bird written in C++ with Qt6**
now with an AI!!! 

<img width="1660" height="620" alt="image" src="https://github.com/user-attachments/assets/3c63b83a-1b89-4b8b-805d-2eeb76e88965" />

Qt-flap now features:
- Spawning a bird
- Spawning many birds
- Spawning a pipe
- Spawning many pipes
- Giving the birds a brain so they stop running into the ground
- Killing birds that run into the first pipe
- Killing other birds that run into pipes
- Giving birds lobotomies
- Giving birds dementia
- A really bad net visualizer!!!

Bird has 4 weights and 1 bias in the `hidden[]` array of the `genome` struct. Birds initially start with a randomized set of values for their weights and bias.
If a bird rams into a pipe it DIES. If a bird survives a bit before ramming into a pipe it DIES. The birds are stuck in an eternal purgatory where they must flap to avoid pipes or die again.
The bird that makes it the furthest passes on its brain + some genetic diversity to the next and gets to survive (although it has to start from the beginning again muahahahahha).
5 birds at the end are then selected to receive dementia. This is just prevention to make sure we always get some different birds (otherwise they'll start ramming into the ceiling and never going through the first pipe, who would've thought a population of 50 would result in very dumb bird?)


As for the program itself:

Birds die to old age after 4000 pipes (i think exhaustion would be the better explanation but too late to change it now!)

**Flags**:
- `--best` to start at bird {0.25f, 0.22f, -2.17f, -0.29f, -0.38f} instead of randomized values

No pytorch was used in the making of Qt-flap (take that python users)
