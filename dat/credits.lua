------------------------------------------------------------------------------[[
-- Filename: credits.lua
--
-- Description: Contains the credits as they are seen on the screen. This file
-- is updated for every Allacrost release to reflect any new additions or to
-- re-arrange the ordering.
------------------------------------------------------------------------------]]

credits_text =
	"Hero of Allacrost Development Team\n" ..
	"\n\n\n" ..

-- Special credit levels
	"Founder and Lead Designer\n\n" ..
	"Tyler Olsen (Roots)\n" ..
	"\n\n" ..

	"Project Leads/Administrators\n\n" ..
	"Tyler Olsen (Roots)\n" ..
	"Jacob Rudolph (rujasu)\n" ..
	"Philip Vorsilak (gorzuate)\n" ..
	"\n\n" ..

-- Main credit level
	"Programming Team\n\n" ..
	"Andy Gardner (ChopperDave) - battle code, menu code\n" ..
	"Baris Soner Usakli (Black Knight) - game editor\n" ..
	"Daniel Steuernol (Steu) - game engine\n" ..
	"Guillaume Anctil (Drakkoon) - map code\n" ..
	"Jacob Rudolph (rujasu) - various game modes, content scripting\n" ..
	"Lindsay Roberts (Linds) - video engine\n" ..
	"Moises Ferrer Serra (byaku) - video engine, audio engine\n" ..
	"Philip Vorsilak (gorzuate) - game editor\n" ..
	"Tyler Olsen (Roots) - programming lead, game engine, map code\n" ..
	"Viljami Korhonen (MindFlayer) - boot code, battle code\n" ..
	"Wouter Lindenhof (ElonNarai) - game editor\n" ..
	"\n\n" ..

	"Artwork Team\n\n" ..
	"Brett Steele (Safir-Kreuz) - map sprites, battle sprites, character portraits, location graphics\n" ..
	"Joe Raucci (Sylon) - battle sprites, old GUI artwork\n" ..
	"Josiah Tobin (Josiah Tobin) - map tiles\n" ..
	"Matthew James (nunvuru) - website graphics, GUI artwork, game logos\n" ..
	"Richard Kettering (Jetryl) - map tiles, map sprites, inventory icons, artwork coordination\n" ..
	"\n\n" ..

	"Music and Sound Team\n\n" ..
	"Jamie Bremaneson (Jam) - sound composer\n" ..
	"Joe Rouse (Loodwig) - soundtrack composer\n" ..
	"Ryan Reilly (Rain) - music and sound lead, soundtrack composer\n" ..
	"Samuel Justice (sam_justice) - sound composer\n" ..
	"Zhe Zhou (shizeet) - sound composer\n" ..
	"\n\n" ..

	"Translation Team\n\n" ..
	"Marcos Avila Isidoro (marcavis) - Portuguese translation\n" ..
	"Yohann Ferreira (Bertram) - French translation\n" ..
	"Andreas Pawlak (nemesis) - German translation\n" ..
	"\n\n" ..

	"Map Designers\n\n" ..
	"Richard Kettering (Jetryl)\n" ..
	"Tyler Olsen (Roots)\n" ..
	"Jacob Rudolph (rujasu)\n" ..
	"\n\n" ..

	"Team Managers\n\n" ..
	"Emre Motan (emrebfg)\n" ..
	"Matt Gallivan (Gallivan)\n" ..
	"\n\n" ..

	"Online Services\n\n" ..
	"Daniel Steuernol (Steu) - wiki support\n" ..
	"Emre Motan (emrebfg) - website content, forum administration\n" ..
	"Heikki Naski (raging_hog) - content management system\n" ..
	"Matt Gallivan (Gallivan) - administration\n" ..
	"Matthew James (nunvuru) - website design\n" ..
	"\n\n" ..

	"Story\n\n" ..
	"Brian Aloisi (Brian Aloisi) - writer, editor\n" ..
	"Tim Hargreaves (Balthazar) - editor\n" ..
	"Tyler Olsen (Roots) - author\n" ..
	"\n\n" ..

	"Packaging\n\n" ..
	"Dmitry Marakasov (AMDmi3) - FreeBSD distribution\n" ..
	"Jorda Polo (ettin) - Debian distribution\n" ..
	"Jacob Rudolph (rujasu) - Windows distribution\n" ..
	"Philip Vorsilak (gorzuate) - OS X distribution\n" ..
	"Tyler Olsen (Roots) - Source distribution\n" ..
	"\n\n" ..

-- Additional credit level
	"Additional Programming\n\n" ..
	"Aaron Smith (Etherstar) - audio engine\n" ..
	"Alastair Lynn (prophile) - various code, Mac OS X maintenance\n" ..
	"Brandon Barnes (Winter Knight) - game engine, game modes, Windows build scripts\n" ..
	"Corey Hoffstein (visage) - battle code\n" ..
	"Dale Ma (eguitarz) - map editor\n" ..
	"DongHa Lee (MoOshiCow) - internationalization\n" ..
	"Farooq Mela (CamelJockey) - video engine\n" ..
	"Kevin Martin (kev82) - video engine, game engine\n" ..
	"Nick Weihs (nickw) - video engine\n" ..
	"Raj Sharma (roos) - video engine\n" ..
	"Vladimir Mitrovic (snipe714) - scripting engine\n" ..
	"(densuke) - various code\n" ..
	"Andreas Pawlak (nemesis) - various code\n" ..
	"\n\n" ..

	"Additional Artwork\n\n" ..
	"Adam Black (indigoshift) - concept art\n" ..
	"Blake Heatly (KaelisEbonrai) - inventory icons\n" ..
	"Chris Hopp (fydo) - map sprite animations\n" ..
	"Chris Luspo (Venndetta1) - concept art\n" ..
	"Francisco Munoz (fmunoz) - inventory icons\n" ..
	"Jason Frailey (Valdroni) - concept art, map sprites\n" ..
	"Jerimiah Short (BigPapaN0z) - map tiles\n" ..
	"John W. Bjerk (eleazar) - map tiles\n" ..
	"Jon Williams (Jonatron) - map sprites\n" ..
	"Herbert Glaser (hollowgrind) - map sprite animations\n" ..
	"Mark Goodenough (RangerM) - map sprite animations, map tiles\n" ..
	"Max Humber (zomby138) - concept art, title screen image\n" ..
	"Nathan Christie (Adarias) - concept art, map sprites, map tiles\n" ..
	"Peter Geinitz (wayfarer) - Concept art, map sprites\n" ..
	"Richard Marks (DeveloperX) - map tiles\n" ..
	"Tyler Olsen (Roots) - various art\n" ..
	"Tyler Stroud (gloomcover) - map tiles\n" ..
	"Vicki Beinhart (Namakoro) - enemy sprites\n" ..
	"Victoria Smith (alenacat) - map sprites, map tiles\n" ..
	"(Jarks) - map tiles, inventory icons\n" ..
	"(Psiweapon) - map tiles, battle sprites\n" ..
	"\n\n" ..

	"Additional Music and Sound\n\n" ..
	"Jean Malary (hamiko) - sound mixer\n" ..
	"Matt Dexter (Star Pilot) - soundtrack composer\n" ..
	"\n\n" ..

	"Additional Internet Services\n\n" ..
	"Felix Kastner (Biohazard) - previous website\n" ..
	"Tim Hargreaves (Balthazar) - previous website, forum administration\n" ..
	"\n\n" ..

	"Additional Translation\n\n" ..
	"Mikko Hanninen (Burnsaber) - Finnish prologue\n" ..
	"\n\n" ..

-- Extra credit level
	"Extra Thanks\n\n" ..
	"(Melchior)\n" ..
	"(Egan1)\n" ..
	"\n\n" ..

-- Special credit level
	"Special Thanks\n\n" ..
	"Daniel Cook (Danc) - Map Tiles\n" ..
	"The Battle for Wesnoth Development Team - Inventory Icons\n" ..
	"(neoriceisgood) - map monster sprites\n"
; -- end credits_text
