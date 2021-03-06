# Wide-Open
Note: after I digged more in Vulkan, I came to a conclusion that this project is very far from optimal and tries to bruteforce OpenGL into Vulkan, which is not how Vulkan is intended to be used, while the project is working and has some good stuff, anything related to commands submission is not, I hope you find it useful though, and check out more recents projects by me regarding Vulkan, I'll try my best to make them much better :)

This project introduces the Wide-Open Common API, which is an extendable API that makes it easier to write stuff in Vulkan once you understand how it works.
Other than the Common API,this project,so far,contains a model for rendering, Blinn-Phong with shadows, which is a very basic single shadow map based shadowing thingy,assets are available too,also a cube map and a (pipeline caching & push constants) implementation are available.
## Building
You must have Meson build system installed on your machine, and the [Prerequisites](#prerequisites) for sure after that,inside root folder do:
```
meson builddir
cd builddir
ninja
```
it will build the libraries and the executable, the libraries will exist inside builddir/subprojects folder,while the executable in builddir
## Switching examples
I've provided ready-to-use functions inside of *main.cpp* which you can use to get a direct render of the example,they are not hard to miss so...
## The Wide-Open Common API
Since there is many repeated coding in Vulkan, and I'll say it,THIS API WILL RESTRICT VULKAN USAGE, it will make a good example for stuff that you might want to set as fixed in your own Vulkan projects,however,with extendablity in mind, if you want to use it while learning Vulkan,I don't think it will be a bad idea,just keep on mind that it is NOT optimized so far
## Blinn-Phong With Shadows
![screenshot](./screenshots/screenshot-1.png)

This model extends The Common API,briefly, it uses two render passes, the first render pass renders the shadow map, a depth image that renders from the light perspective, and the second render pass that uses a basic framebuffer with a single color image, and a single depth image,the second render pass uses the image rendered in the first render pass to sample depth values from and decide which fragments are being shadowed, it also uses the Blinn-Phong system in the second render pass which is described below.
## Cubemap
A VERY simple cube map implementation, it draws nothing but the cube map, it uses a 6-layer image for sampling which you provide from your disk (or any other way really if you want to),depth testing is off, actually the whole depth image/buffer is not there (You can turn it on with some Vulkan coding though),no mouse movement detection,no uniforms updating at runtime (if you change your ubo.MVP between renders it won't do anything (DO NOT CALL applyUBO between renders it would give an error, it is suppossed to))
## Pipeline Caching and Push Constants
A very simple example that uses both implementations,since they are not directly related, pipeline caching will store a cache file on your storage device after first run, and it'll start using it the next time you start the app,unless you delete it,note that this is only valid for this subproject, push constants simply pushes an MVP (a transform) matrix into a simple shader and uses it,the shader is available at [Assets Folder](https://github.com/RedDeadAlice/Wide-Open/tree/master/Assets), code is commented too.
## Blinn-Phong (REMOVED)
This subproject implements the Blinn-Phong lighting system into Vulkan API,it simply uses a single color attachment , and a depth attachment for depth testing,the shaders are simple and straightforward, the shaders recieve vertices position and normal data, and outputs a single color per fragment, no textures support at all,and there is only a single light and a single mesh (which I think it'll be easy to expand by a few changes).
Note: this model doesn't use the most of the Common API (it was made before it was refined, so basically,this project is more Pure-Vulkan than a Common-extended project)
## Contribution
This project is open for contribution, you can create more examples or add improvements on the Common API, it is recommended, however,to use the Common API when writing new examples,when extending the Common API, please try to keep your namings clean, also Meson should be used as the build system, it will make it easier for everyone to get things done.
## Prerequisites
- [Assimp](https://www.assimp.org/) library
- [Vulkan SDK](vulkan.org)
- [GLFW](https://www.glfw.org/) library
- [GLM](https://github.com/g-truc/glm) library
- [lodepng](https://lodev.org/lodepng/) library (YOU MAY NEED TO COMPILE IT WITH -fPIC flag,also check meson.build inside Cubemap subproject when facing errors,it is sat to locate the library from subprojects/Cubemap, but you can change it if that's what you want)
### [Assets Folder](https://github.com/RedDeadAlice/Wide-Open/tree/master/Assets)
Used Meshes,textures,and shaders in both GLSL and SPIR-V,cheers.
## Docs
The Vulkan specs is a great place to read from if you didn't know what a Vulkan function is for.
The Wide-Open Common API docs can be found [here](https://github.com/RedDeadAlice/Wide-Open/blob/master/subprojects/Common/docs/Wide-Open%20Common%20API.pdf), Blinn-Phong With Shadows, and Cubemap example are commented with assuming that you've read the Wide-Open Common API docs, pure Vulkan is and will NOT be explained since it IS explained in the Vulkan specs.
### Optimization
This project is far from being robust and lacks tons of optimizations,BUT it's good for Vulkan beginners to understand the concept of using Vulkan.
## License
This project is under MIT License, check out [LICENSE](https://github.com/RedDeadAlice/Wide-Open/blob/master/LICENSE).
### Made with love and human-sized kittens
