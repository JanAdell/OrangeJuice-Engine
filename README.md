# Orange Juice Engine
Repository dedicated to the development and releases of our home brew engine, Orange Juice. This project is an assignment for the 3D Engines subject at the Videogame Design and Development degree (CITM, UPC). The engine is created with C++ and other major libraries such as OpenGL and ImGui.

This project is developed by Oscar Reguera and Jan Adell. 

## How to Use

### Controls
* **WASD** Keys for camera movement
* **Spacebar** to bring the camera up
* **Left Control** to bring the camera down
* **Mouse Wheel** to zoom in and out
* **ALT + Left mouse click** to rotate the camera
* **F key** to focus on an object

### General Use

Just download our latest release either in our repository's releases page or clicking [here](https://github.com/JanAdell/OrangeJuice-Engine/releases), unzip the file, and execute **OrangeJuice_Engine.exe**.

From there feel free to play around with the UI, load any models (either the ones on the Assets folder or your own) or do any action you want. Feel free, if you encounter an error, to report it [here](https://github.com/JanAdell/OrangeJuice-Engine/issues).

**When a Game Object is loaded, the inspector must be opened and we need to select their childs (Gameobjects in drop down menu within Hirearchy) in order to change textures or act on them**

### Known errors

* Some specific FBX models might not load into the engine due to their version. Loading them into 3DS Max and reimporting to a newer version allows for them to be seen in the engine.

* Primitives don't load textures properly. We can see them change when a texture is dropped when one is selected but we have trouble prositioning the texture into the primitive.

* Inpector/Hirearchy tab may show the parent game object selected, yet it's not. The selected game object is that which is selected from the drop down menu: 
> For example, if we have a "GameObject 1" which holds a "GameObject 1.1" within its drop down menu, when "GameObject 1.1" is highlighted in orange and we see its Inspector window, "GameObject 1.1" is selected. If we move to "GameObject 2.1", within "GameObject 2" and select it, we may still see "GameObject 1" highlighted, but it's just a visual error and we really have "GameObject 2.1" selected. 

### Future Implementation

* Allow the user to move the models in the scene either through inspector or with their mouse if the object is selected. We wanted to allow position changes on this first release, but it was giving us serious problems, so we have decided to leave it as something for display only, and work to have it ready as intended for the next release. 
* Allow changes to be made not only to childs of the parent game object (which right now is an empty container) but to the parent itself so we dont have to work child by child. 

## Features

* Load FBX models
* Load Textures into said models
* Console which logs the loading process
* Hardware tracking and performance tracking
* Hirearchy system which allows to work with models with multiple meshes (which appear as children in the inspector tab)

## Authors

> [Jan Adell](https://github.com/JanAdell)

> [Oscar Reguera](https://github.com/oscarrep)

## [Project's Repository](https://github.com/JanAdell/OrangeJuice-Engine)

## License

Copyright (c) 2021 Jan Adell Mill & Oscar Reguera Parera

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files(the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions :

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
