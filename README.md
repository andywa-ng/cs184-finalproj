# Rain It In

A physically-based rain simulation that dynamically adjusts to rain intensity and droplet size. 

## Description

Rain is a complex interaction between water and its surface. We want to simulate rain that adapts to current rain
conditions: heavier rain should create bigger splashes or rain falling on objects should either bounce or drip based off
of geometry and intensity. Drop physics, adjustable parameters and splash simulation would all be taken account when
creating this project. We will treat each rain droplet as its own particle and apply the proper forces for it to behave as such. 
Figuring out the math for splash and pooling effects might be challenging, but nothing a little physics cant handle.

The project will combine particle-based simulation with real-time rendering techniques to visually and physically simulate
realistic rain behavior and interaction with surfaces including itself. 


## Getting Started

### Dependencies

* Describe any prerequisites, libraries, OS version, etc., needed before installing program.
* ex. Windows 10

### Executing program

* How to run the program

1. Compile the program inside under `/3d_sim`

```
make
```

2. Run the executable file
```
./3d_simulation
```

### GUI Commands

* Press `R` to restart the animation
* Press `P` to pause the animation, and again to resume
* Use  `W`, `A`, `S`, `D` to move the camera around the plane

## Authors

* Sophie Nazarian 
* Andy Wang 
* Dmytro Krukovskyi 
* April Zhang 


## Acknowledgments

Inspiration, code snippets, etc.
* [Physically Based Modeling and Animation of Fire](https://www.cs.columbia.edu/cg/pdfs/135-splash_egsr07.pdf)
* [GPU-based Rain Simulation](https://classes.cs.uchicago.edu/archive/2022/fall/23700-1/papers/gpu-rain.pdf)
* [Matplotlib docs for animation:](https://matplotlib.org/stable/api/animation_api.html)
* [Physically Based Modeling and Animation of Fire](https://www.cs.columbia.edu/cg/pdfs/135-splash_egsr07.pdf)
* [Previous Student Project on Water Simulation](https://cal-cs184-student.github.io/project-webpages-sp23-CardiacMangoes/finalProject/index.html)