# XMinecraft

The goal of this project is to run Minecraft Beta 1.7.3 on the original Xbox. I'm using [rxdk](https://github.com/Team-Resurgent/RXDK)
and will eventually also use [rxgl](https://github.com/Team-Resurgent/RXGL) to make the development experience nicer.

I'm not sure why I'm doing this, I probably just thought it would be funny and a neat flex.

## Progress

- [x] Class file parsing
- [x] Jar file parsing
- [x] Bytecode parsing
- [ ] JIT compiler (currently being designed)
- [ ] Class linking
- [ ] Runtime environment
- [ ] JRE reimplementation
- [ ] Native reimplementation
- [ ] Optimization

## JIT

I'm planning to use a static single assignment IR when translating bytecode into native code, just like HotSpot and most compilers.
I've started making some basic control flow graph stuff.

