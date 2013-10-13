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

1) Run Converter.UI.exe.
2) Set the paths correctly.
	If the browse buttons don't work, type something into the text fields and try again.
3) Set the options how you want them.
4) Go to the convert tab.
5) Click 'save configuration file'.
6) Click Convert.
7) When the converter finishes, load up the save and verify everything converted well.
	


----
INSTRUCTIONS (manual method)
----

1) Find the desired save file in "[YOUR CK2 SAVE DIRECTORY]/save games".
2) Open the file "configuration.txt".  In the file there is a line that reads 
	CK2directory = "D:\Paradox Interactive\Crusader Kings II"
   Modify it so that it points to YOUR copy of Crusader Kings 2.
   If using Steam, this is likely "C:\Program Files\Steam\steamapps\common\Crusader Kings II\"
   Otherwise it is likely "C:\Program Files\Paradox Interactive\Crusader Kings II\"
3) In "configuration.txt", there is a line that reads
	EU3directory = "D:\Paradox Interactive\Europa Universalis III"
   Modify it so that it points to YOUR copy of Europa Universalis III.
   If using Steam, this is likely "C:\Program Files\Steam\steamapps\common\Europa Universalis III\"
   Otherwise it is likely "C:\Program Files\Paradox Interactive\Europa Universalis III\"
4) Run "CK2ToEU3.exe" by dropping your save onto the exe.  This will create a file in the converter's directory called "output.eu3".
5a) If you are not using the converter mod, copy the new save to "[YOUR EU3 DIRECTORY]/save games".
5b) If you are using the converter mod, copy the items in the new mod folder to "[YOUR EU3 DIRECTORY]/mod".
6) A file named log.txt will also be created. Immediately report any errors to the thread on the forums.
7) Load up the save and verify everything converted well.



----
CREDITS
----

An (incomplete) list of those who contributed effort to this mammoth undertaking

Idhrendur	- Programming, Project Lead
dtremenak	- Programming, Analysis, Tools
kvess		- Frontend
DasGuntLord01	- Analysis, Data Files
Puking Panda	- Analysis
Pode		- Analysis
johnjohansson	- Analysis
M0rdred		- Analysis
GreyHuge	- Tools
tamius23	- Data Files
rho		- Data Files
Gimenez		- Data Files
birdboy2000	- Data Files
Comradebot	- Data Files
mrguymiah	- Data Files

 - AND ALL THE OTHERS WHO GAVE THEIR INPUT ON THE FORUM -