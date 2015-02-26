This program will convert a Europa Universalis 4 save game to a Victoria 2 save game.

----
IMPORTANT LINKS
----

Forum thread: http://forum.paradoxplaza.com/forum/showthread.php?707362-The-EU4-to-V2-Converter-Project
Sourceforge project: https://sourceforge.net/projects/eu4tov2/

----
REQUIREMENTS
----
V2 - HoD required
EU4

-----
INSTRUCTIONS
-----
1. Run ConverterFrontend.exe.
2. Fill out all fields and keep clicking Next.


----
MANUAL METHOD INSTRUCTIONS
----

1. Download the converter and unzip it wherever.
2. Copy your EU4 save into the converter directory.
3. Edit configuration.txt to point at your game installs, and to set any options you care about.
4. Drag and drop your save onto EU4toV2Converter.exe
5. Go into the output folder, then the subfolder named after your save.
6. Copy the folder named after your save and the corresponding mod file into your V2 mod folder.
7. Start that copy of Victoria 2, and select the mod.

----
FAQ
----
Q: I got an error that mentions MSVCP120.dll. What do I do?
A : Go to the Visual Studio Redistributable page: http://www.microsoft.com/en-us/downl....aspx?id=40784
then click Download and select vcredist_x86.exe.
Once it has downloaded, install it and run the converter again.

Q: I have an ironman save. Can it be converted?
A: Well in fact you can transform an ironman save into a normal save.
1. Locate your ironman save on your computer. If it's on the cloud, there is a copy there : ..\Steam\userdata\<your account>\236850\remote\save games
2. Copy it on the "local" save game directory and edit the copy with a software (notepad++ may be very useful)
3. On the first line you will found the three letters of the tag of your nation (BYZ for example)
4. Replace it with another : (FRA is a good one because the BBB is always around)
5. Load the save. You will play as France
6. Save it, change again the nation on the new save
7. Load the save again and then save. This time you will notice that it's asking which name giving to this save, which implies it is no more ironman.
8. TADAM ! It's no more ironman but a regular save
Thanks to Helkoi for the technique!

Q: I loaded my mod, but nothing changed. What's wrong?
A: You probably placed the mod in the My Documents mod folder. It needs to go in the Vic2 install location's mod folder.

Q: Vic2 is crashing at converting history. What do I do?
A: Try going into the mod and deleting history/units. If it now loads, likely a navy got placed badly. Open up log.txt and search for warnings. If any mention whitelisting and blacklisting, open port_blacklist.txt and add the provinces in the warnings to file file. Save it and reconvert. Repeat checking for the warnings and adding the provinces until there are no more warnings. You should now get a working mod. Make sure to inform the team of any provinces you had to add so we can include them in future releases.


Q: Vic2 is crashing at Initializing Maplogic. What do I do?
A: Clear your map cache by deleting the cache folder inside ".../victoria 2/map/" in both My Documents and the Victoria 2 install location.

Q: The converter crashes after no time with no errors and no log.txt (when running through the frontend). What went wrong?
A: This is the MSVCP120.dll error from earlier in the FAQ. Use that solution.



----
CREDITS
----

An (incomplete) list of those who contributed effort to this mammoth undertaking

Idhrendur		- Project Lead, Programming
Ztaesek			- Programming, Data Files
theosZA			- Programming
PTSnoop			- Programming, Graphics, Analysis
kwsdev			- Front End
TheSpartanD		- Data Files
LordBawbag		- Data Files
MamelukeSkywalker	- Data Files
LordOfTurtles		- Data Files
localtoast		- Data Files
G_Morgan		- Data Files
rageair			- Data Files
niallmcfc		- Data Files
plnp123			- Data Files
Goldmattress		- Data Files
Torakka			- Data Files
LowTartare		- Graphics
ChortlingGnome		- Graphics
Magc8Ball		- Graphics
Popcorn Soda55		- Graphics


 - AND ALL THE OTHERS WHO GAVE THEIR INPUT ON THE FORUM -