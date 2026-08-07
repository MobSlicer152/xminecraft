# XMinecraft

The goal of this project is to run Minecraft Beta 1.7.3 on the original Xbox. I'm using [rxdk](https://github.com/Team-Resurgent/RXDK)
and will eventually also use [rxgl](https://github.com/Team-Resurgent/RXGL) to make the development experience nicer.

I'm not sure why I'm doing this, I probably just thought it would be funny and a neat flex.

## JVM

Initially, I had hoped to port HotSpot or OpenJ9, but they're both heavily designed around dynamic linking.
Instead, for the sake of learning, simplicity (ha), and as a flex, I'm just writing my own Java 6 VM. So far
I'm about two thirds done parsing class files, and I plan to get some basic bytecode processing done after that.
I might also try JIT, if it's feasible.

Security and class file validation isn't really a priority, the point is to run known-good code from Minecraft.

## Base game modifications

I plan to add controller support, and try to optimize the game's memory usage as much as I can. Hopefully that makes
it an actually pleasant experience. If necessary, I'll rewrite the raw OpenGL 1.1 rendering code with D3D8, but that
sounds pretty time consuming. Hopefully I can also make mods like Better Than Adventure work too.

