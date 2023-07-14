# opengl-training

| Activities | Description |
| :---: | :--- |
| 🗂 ACT1 | Basic Transformations in XY axis and shaders. |
| 🗂 ACT2 | 3D model, Camera Transformations and Euler Angles in XYZ axis. |
| 🗂 ACT3 | Phong Model, Multiple Light Focus and integration with Qt interfaces. |

| Activity | Result | Interaction Mouse/Keys |
| :---: | :---: | --- |
| <h3> 1 </h3> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/37f58f34-2a93-46ac-b896-4de605ce00e0" alt="ACT1" width="300"/> | Ⓒ: Crown Appears <br /> Ⓒ: Red Striped shirt <br /> Ⓡ: Rotate Y Axis |
| <h3> 2 </h3> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/252756d9-fd32-4124-afe3-a628262eea60" alt="ACT2" width="300"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br />  ⇧: Move Morty Foward <br /> ⇩: Move Morty Backward <br /> ⇦: Rotate Morty 45° Y axis <br /> ⇨: Rotate Morty -45° Y axis <br />  Ⓒ: Toggle Morty's 1st Person Cam/Euler Angles Cam <br /> Ⓠ: Rotate Rick 45° <br /> Ⓔ: Rotate Rick -45° <br /> Ⓟ: Portal appears in front of him <br /> Ⓟ: Close Portal in front of him. <br /> Ⓡ: Restart Scene <br /> ▫ New Morty Appears if he walks through the portal |
| <h3> 3 </h3> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e40026a5-a1a0-4b91-a6fa-368afa0008a9" alt="ACT3" width="370"/> | <ins> Mouse/Keys </ins>  <br /> <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br /> Ⓦ/Ⓢ: Move active candle Up and Down <br /> Ⓐ/Ⓓ: Move active candle Left and Right <br /> Ⓣ: Toggle active candle <br /> Ⓡ: Rotate active candle's Snitch <br />  <br /> <ins> Qt Interfaces </ins> <br /> ▹ Radio Buttons: Toggle between left and right candle <br /> ▹ On/Off Button: Toggle On/Off Active candle focus light <br /> ▹ Dial: Modify Rotation Angle of active candle <br />|

#

| Exams Folder | Year |
| :---: | :--- |
| 🗂 Exams/EXAM1 | Examen OpenGL 2018-19 |
| 🗂 Exams/EXAM2 | Examen OpenGL 2020-21 |
| 🗂 Exams/EXAM3 | Examen OpenGL 2021-22 |
| 🗂 Exams/EXAM4 | Examen OpenGL 2022-23 |

| Exam | Initial Scene | Result | Interaction Mouse/Keys |
| :---: | :---: | :---: | --- |
| <h3> 1 </h3> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/900d9c14-b744-46d1-bc32-ceb78f66c59a" width="300"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/b03c962b-55a1-49d4-822d-1a23055d9cd2" width="300"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br /> (To do...)|
| <h3> 2 </h3> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/dad9e9a7-e121-4e5a-a3e9-a6d60e0fbda4" width="300"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/9e0abff0-a5c5-45f5-a7ab-24d5282eb624" width="300"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br /> (To do...)|
| <h3> 3 </h3> | <img src="" width="300"/> | <img src="" width="370"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br /> (To do...)| 
| <h3> 4 </h3> | <img src="" width="300"/> | <img src="" width="370"/> | <img src="https://github.com/artHub-j/opengl-training/assets/92806890/e9c75409-2c57-4e35-bbd8-d7db0ac92362" width="15"/>: Move Euler Angles Camera <br /> (To do...)| 

# Previous steps and generation of executables

<h3> 1. Installing Qt </h3>
Check if you already have a version of Qt installed:

```
qmake --version
```

If not, install Qt-5 using the following commands:

```
sudo apt-get install build-essential
sudo apt-get install qtcreator
sudo apt-get install qt5-default
```

<h3> 2. Check OpenGL and install GLM (OpenGL Mathematics): </h3>

Install and validate that OpenGL is installed correctly in your graphics card's driver:

```
sudo apt install mesa-utils
glxinfo | grep -E "direct rendering|^OpenGL"
```

Your terminal output should be similar to this:

<img src="https://github.com/artHub-j/opengl-training/assets/92806890/eb73da74-439c-43fe-85e5-06b17a87c1e7" width="500"/>

Then, install cmake (metacompiler), build-essential (package that includes the necessary to develop in C++) <br />
and libglm-dev (GLM library):

```
sudo apt-get install cmake build-essential libglm-dev
```

<h3> 3. Generate executables: </h3> 

```
cd directory_act_ex/
qmake
make
./executable_name
```
