# Unreal Engine Plugin - MultiplayerSessions

A plugin for Unreal Engine that streamlines the process of creating and joining online game sessions targeting different platforms.

This plugin was created as part of the [Unreal Engine 5 C++ Multiplayer Shooter](https://www.udemy.com/course/unreal-engine-5-cpp-multiplayer-shooter/?couponCode=KEEPLEARNING) course on Udemy.

<br>

## Setup

The following instructions outline the general process for adding new plugins to your project.

1. Inside of your Unreal Engine project, create a new `Plugins` directory (if one does not exist already).

2. Clone this repository into the `Plugins` directory.

3. Inside of the Unreal Engine editor for your project, navigate to `Edit->Plugins`, and enable the `MultiplayerSessions` plugin:

    ![image](https://github.com/user-attachments/assets/e882bed5-aefa-49f7-a21f-6ebc77a91d75)

<br>

4. In the `.Build.cs` file for your project, add `MultiplayerSessions` to the list of modules:

    ![image](https://github.com/user-attachments/assets/9ce7fbfa-2aba-4002-88db-9064d8c4fa8c)

<br>

5. Regenerate your Visual Studio project files by deleting the `Saved`, `Intermediate`, and `Binaries` directories, right-clicking on your `.uproject` file, and selecting "*Generate Visual Studio project files*": 

    ![image](https://github.com/user-attachments/assets/a5a302ac-c945-4300-9267-4bc1ab50682a)