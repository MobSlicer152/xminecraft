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

I've been reading about how JIT compilers and bytecode interpreters generally work, and designing my own.
It's probably gonna work something like this:

- Scan for beginnings of control flow blocks
- Scan for ends of control flow blocks
- Create and process a control flow graph with inbound and outbound operand stacks
- Translate to final x86 machine code

