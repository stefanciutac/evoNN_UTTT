# evoNN_UTTT: The Neuroevolution Algorithm Trained on Ultimate Tic-Tac-Toe (v0.1.2)

This project is an attempt to create a problem-agnostic, efficient, and performant neuroevolution library. It is tested and developed, initially, for the board game of Ultimate Tic-Tac-Toe, as it is a non-trivial, non-differentiable, complete-information problem.

**GitHub Pages Devlog: https://stefanciutac.github.io/stefanciutac_devlog/**

The devlog linked above contains posts discussing system architecture and the rationale behind the design decisions I have made, as well as discussing the project's next steps (the current latest post analyses v0.1.0). The features which will be implemented soon are listed at the bottom of each post.

### Notes:
- **The project is currently a work in progress, and far from a first stable release**; it is still in the initial implementation stage, rather than being merely refined
- Although the board game of Tic-Tac-Toe, and later, Ultimate Tic-Tac-Toe is used to develop and benchmark the alpha versions and first implementation of the system, the resulting library aims to be problem-agnostic
- The next intended steps of development are:
    1. The implementation of genetic diversity logging (using the standard deviation of the fitness scores)
    2. The design and implementation of a (marginally) less ineffective selection algorithm
    3. The optimisation of hyperparameters - using a brute force search, or perhaps through the implementation of NEAT
    4. The restructuring of data flow within the codebase for maintainability and data generality

### System Prerequisites:
- The Eigen3 library must be installed
