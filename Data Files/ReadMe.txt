This program will convert a Crusader Kings 2 save game to a Europa Universalis 3: Divine Wind save game.

----
IMPORTANT LINKS
----

Forum thread: http://forum.paradoxplaza.com/forum/showthread.php?587798-The-CK2-To-EU3-Converter-Project
Sourceforge project: https://sourceforge.net/projects/ck2toeu3/

----
REQUIREMENTS
----
CK2 version 1.02 or later
EU3:DW version 5.1 or later


----
INSTRUCTIONS
----

1) Find the desired save file in "[YOUR CK2 SAVE DIRECTORY]/save games".
2) Copy the save game in to the directory of this converter, and rename it to "input.ck2"
3) Open the file "configuration.txt".  In the file there is a line that reads 
	CK2directory = "D:\Paradox Interactive\Crusader Kings II"
   Modify it so that it points to YOUR copy of Crusader Kings 2.
   If using Steam, this is likely "C:\Program Files\Steam\steamapps\common\Crusader Kings II\"
   Otherwise it is likely "C:\Program Files\Paradox Interactive\Crusader Kings II\"
4) In "configuration.txt", there is a line that reads
	EU3directory = "D:\Paradox Interactive\Europa Universalis III"
   Modify it so that it points to YOUR copy of Europa Universalis III.
   If using Steam, this is likely "C:\Program Files\Steam\steamapps\common\Europa Universalis III\"
   Otherwise it is likely "C:\Program Files\Paradox Interactive\Europa Universalis III\"
5) Run "CK2ToEU3.exe".  This will create a file in the converter's directory called "output.eu3".
6) Copy output.eu3 to "[YOUR EU3 DIRECTORY]/save games".
7) A file named log.txt will also be created. Immediately report any errors to the thread on the forums.
8) Load up the save and verify everything converted well.



----
CREDITS
----

A (incomplete) list of those who contributed effort to this mammoth undertaking

Idhrendur	- Programming
DasGuntLord01	- Analysis, Data Files
dtremenak	- Analysis, Tools
Puking Panda	- Analysis
GreyHuge	- Tools
tamius23	- Data Files
rho		- Data Files

 - AND ALL THE OTHERS WHO GAVE THEIR INPUT ON THE FORUM -