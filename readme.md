LevelDB Plugin for Unreal Engine 4
=============

Welcome this plugin makes use of:
* LevelDB by Google
* Snappy by Google

I've included precompiled Libraries, those were compiled for 64-bit only. So curently the plugin wont work on 32 bit builds. Also included are some needed boost libs


There are 5 functions Exportet to Blueprint
* Open Database
* Write String to Database
* Write Object to Database
* Read String from Database
* Read Object from Database

... all located unter the LevelDB Category
		

Corresponding C++ functions can be looked up in the FunctionLibrary Source file