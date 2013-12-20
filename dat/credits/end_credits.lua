------------------------------------------------------------------------------[[
-- Filename: episode1_end_credits.lua
--
-- Description: Contains the credits seen at episode I end.
-- It shall be moved from an episode end to another until reaching the game end.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
end_credits = ns;
setfenv(1, ns);

local credit_map = {}
local credit_id = 0;
local credit_time = 0;
local text_alpha = 1.0;

local dark_overlay = {};
local overlay_time = 0;

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    credit_map = {
        [0] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Thanks for playing!!"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 384.0,
            header_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
            text = Script:CreateText(vt_system.Translate("(To be continued ...)"), vt_video.TextStyle("text22")),
            text_position_x = 670.0,
            text_position_y = 548.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [1] = {
            image = Script:CreateImage("img/logos/valyria_logo_black.png"),
            image_dimension_x = 630.0,
            image_dimension_y = 318.0,
            image_position_x = 500.0,
            image_position_y = 500.0,
            header = nil,
            text = Script:CreateText(vt_system.Translate("Episode I"), vt_video.TextStyle("text28")),
            text_position_x = 670.0,
            text_position_y = 548.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [2] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Story"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Bertram\n\nLilou", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [3] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Lead Programming, Scripting, Maps"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Bertram", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [4] = {
            image = Script:CreateImage("img/portraits/kalya_full.png"),
            image_dimension_x = 182.0,
            image_dimension_y = 350.0,
            image_position_x = 200.0,
            image_position_y = 620.0,
            header = Script:CreateText(vt_system.Translate("Additional programming and scripting"), vt_video.TextStyle("text28")),
            header_position_x = 510.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Roots\n\nIkarusDowned\n\nCoderGreen\n\nAuthenticate\n\nLogzero\n\nEndoalir", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [5] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Episode I - Graphics"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Daniel Cook\n\nZabin\n\nSafir-Kreuz\n\nJetrel\n\nOpen Game Art\n\nRamtam\n(vectorgurl.com)\n\nPatrick Thompson\n(pkillustration.com)\n"
                                    .."(WIP)", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [6] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Music"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Mathew Pablo\n(matthewpablo.com)\n\nZander Noriega (OGA)\n\nKoertes (OGA)\n\nHorrorPen (OGA)",
                                     vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [7] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Ports"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Ablu\n\nSocapex\n\nIbara\n\nAMDmi3", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [8] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Translations"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("NaN (de)\n\nGallaecio (gl)\n\nBioHazardX (it)\n\nKnitter (pt_PT)\n\nBertram (fr)", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [9] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Tester Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Shirish\n\nKhyana\n\nSahaondra\n\nOmega\n\nPenPen", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [10] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Valyria Tear was forked from Hero of Allacrost in 2011"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 384.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = nil,
        },
        [11] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Development Team (2004-2011)\n\nFounder and Lead Designer"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Tyler Olsen (Roots)", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [12] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Project Leads/Administrators"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Tyler Olsen (Roots)\n\nJacob Rudolph (rujasu)\n\nPhilip Vorsilak (gorzuate)", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [13] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Programming Team (2004-2011)"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Andy Gardner (ChopperDave) - battle code, menu code\n\n" ..
                                     "Baris Soner Usakli (Black Knight) - game editor\n\n" ..
                                     "Daniel Steuernol (Steu) - game engine\n\n" ..
                                     "Guillaume Anctil (Drakkoon) - map code\n\n" ..
                                     "Jacob Rudolph (rujasu) - various game modes, content scripting\n\n" ..
                                     "Lindsay Roberts (Linds) - video engine\n\n" ..
                                     "Moises Ferrer Serra (byaku) - video engine, audio engine\n\n" ..
                                     "Philip Vorsilak (gorzuate) - game editor\n\n" ..
                                     "Tyler Olsen (Roots) - programming lead, game engine, map code", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [14] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Programming Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Viljami Korhonen (MindFlayer) - boot code, battle code\n\n" ..
                                     "Wouter Lindenhof (ElonNarai) - game editor\n\n" ..
                                     "Aaron Smith (Etherstar) - audio engine\n\n" ..
                                     "Alastair Lynn (prophile) - various code, Mac OS X maintenance\n\n" ..
                                     "Brandon Barnes (Winter Knight) - game engine, game modes, Windows build scripts\n\n" ..
                                     "Corey Hoffstein (visage) - battle code\n\n" ..
                                     "Dale Ma (eguitarz) - map editor\n\n" ..
                                     "DongHa Lee (MoOshiCow) - internationalization\n\n" ..
                                     "Farooq Mela (CamelJockey) - video engine\n\n", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [15] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Programming Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Kevin Martin (kev82) - video engine, game engine\n\n" ..
                                     "Nick Weihs (nickw) - video engine\n\n" ..
                                     "Raj Sharma (roos) - video engine\n\n" ..
                                     "Vladimir Mitrovic (snipe714) - scripting engine\n\n" ..
                                     "(densuke) - various code\n\n" ..
                                     "Andreas Pawlak (nemesis) - various code\n", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [16] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Artwork Team (2004-2011)"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Brett Steele (Safir-Kreuz) - map sprites, battle sprites, character portraits, location graphics\n\n" ..
                                     "Joe Raucci (Sylon) - battle sprites, old GUI artwork\n\n" ..
                                     "Josiah Tobin (Josiah Tobin) - map tiles\n\n" ..
                                     "Matthew James (nunvuru) - website graphics, GUI artwork, game logos\n\n" ..
                                     "Richard Kettering (Jetryl) - map tiles, map sprites, inventory icons, artwork coordination", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [17] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Adam Black (indigoshift) - concept art\n\n" ..
                                     "Blake Heatly (KaelisEbonrai) - inventory icons\n\n" ..
                                     "Chris Hopp (fydo) - map sprite animations\n\n" ..
                                     "Chris Luspo (Venndetta1) - concept art\n\n" ..
                                     "Francisco Munoz (fmunoz) - inventory icons\n\n" ..
                                     "Jason Frailey (Valdroni) - concept art, map sprites\n\n" ..
                                     "Jerimiah Short (BigPapaN0z) - map tiles\n\n" ..
                                     "John W. Bjerk (eleazar) - map tiles\n\n" ..
                                     "Jon Williams (Jonatron) - map sprites", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [18] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Herbert Glaser (hollowgrind) - map sprite animations\n\n" ..
                                     "Mark Goodenough (RangerM) - map sprite animations, map tiles\n\n" ..
                                     "Max Humber (zomby138) - concept art, title screen image\n\n" ..
                                     "Nathan Christie (Adarias) - concept art, map sprites, map tiles\n\n" ..
                                     "Peter Geinitz (wayfarer) - Concept art, map sprites\n\n" ..
                                     "Richard Marks (DeveloperX) - map tiles\n\n", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [19] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Tyler Olsen (Roots) - various art\n\n" ..
                                     "Tyler Stroud (gloomcover) - map tiles\n\n" ..
                                     "Vicki Beinhart (Namakoro) - enemy sprites\n\n" ..
                                     "Victoria Smith (alenacat) - map sprites, map tiles\n\n" ..
                                     "(Jarks) - map tiles, inventory icons\n\n" ..
                                     "(Psiweapon) - map tiles, battle sprites", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [20] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Music and Sound Team (2004-2011)"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Jamie Bremaneson (Jam) - sound composer\n\n" ..
                                     "Joe Rouse (Loodwig) - soundtrack composer\n\n" ..
                                     "Ryan Reilly (Rain) - music and sound lead, soundtrack composer\n\n" ..
                                     "Samuel Justice (sam_justice) - sound composer\n\n" ..
                                     "Zhe Zhou (shizeet) - sound composer\n\n" ..
                                     "Jean Malary (hamiko) - sound mixer\n\n" ..
                                     "Matt Dexter (Star Pilot) - soundtrack composer", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [21] = {
            image = nil,
            header = Script:CreateText(vt_system.Translate("Allacrost Special Thanks"), vt_video.TextStyle("text28")),
            header_position_x = 512.0,
            header_position_y = 200.0,
            header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
            text = Script:CreateText("Daniel Cook (Danc) - Map Tiles\n\n" ..
                                     "The Battle for Wesnoth Development Team - Inventory Icons\n\n" ..
                                     "(neoriceisgood) - map monster sprites\n\n" ..
                                     "(Melchior)\n\n" ..
                                     "(Egan1)", vt_video.TextStyle("text24")),
            text_position_x = 512.0,
            text_position_y = 300.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
        [22] = {
            image = nil,
            header = nil,
            text = Script:CreateText(vt_system.Translate("And you..."), vt_video.TextStyle("text28")),
            text_position_x = 512.0,
            text_position_y = 384.0,
            text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
        },
    }

    credit_id = 0;
    credit_time = 0;
    set_image_dimensions = false;

    dark_overlay = Script:CreateImage("img/ambient/black.png");
    dark_overlay:SetDimensions(1024.0, 768.0);
    overlay_time = 0;
end

function Update()
    if (GlobalManager:GetEventValue("game", "Start_End_Credits") == 0) then
        return;
    end

    -- Stop once the credits have finished.
    if (credit_map[credit_id] == nil) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the timers
    credit_time = credit_time + time_expired;
    if (overlay_time <= 2500) then
        overlay_time = overlay_time + time_expired;
    end

    -- Start the timer
    if (credit_time > 7000) then
        credit_time = 0

        credit_id = credit_id + 1;
        if (credit_map[credit_id] ~= nil and credit_map[credit_id].image ~= nil) then
            credit_map[credit_id].image:SetDimensions(credit_map[credit_id].image_dimension_x, credit_map[credit_id].image_dimension_y);
        end

    end

    if (credit_time >= 0
            and credit_time <= 1000) then
        text_alpha = credit_time / 1000;
    elseif (credit_time > 6000
            and credit_time <= 7000) then
        text_alpha = 1.0 - (credit_time - 6000) / (7000 - 6000);
    elseif (credit_time > 7000) then
        text_alpha = 0.0;
    end

end

local image_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local overlay_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Apply a dark overlay first.
    if (overlay_time <= 2500) then
        local overlay_alpha = overlay_time / 2500;
        overlay_color:SetAlpha(overlay_alpha);
    end
    VideoManager:Move(512.0, 768.0);
    dark_overlay:Draw(overlay_color);

    -- Stop once the credits have finished.
    if (credit_map[credit_id] == nil) then
        return;
    end

    if (GlobalManager:GetEventValue("game", "Start_End_Credits") == 0) then
        return;
    end

    -- Display potential images
    if (credit_map[credit_id].image ~= nil) then
        image_color:SetAlpha(0.8 * text_alpha);
        VideoManager:Move(credit_map[credit_id].image_position_x, credit_map[credit_id].image_position_y);
        credit_map[credit_id].image:Draw(image_color);
    end

    -- Header
    if (credit_map[credit_id].header ~= nil) then
        credit_map[credit_id].header_color:SetAlpha(1.0 * text_alpha);
        VideoManager:Move(credit_map[credit_id].header_position_x, credit_map[credit_id].header_position_y);
        credit_map[credit_id].header:Draw(credit_map[credit_id].header_color);
    end
    -- Text
    if (credit_map[credit_id].text ~= nil) then
        credit_map[credit_id].text_color:SetAlpha(0.9 * text_alpha);
        VideoManager:Move(credit_map[credit_id].text_position_x, credit_map[credit_id].text_position_y);
        credit_map[credit_id].text:Draw(credit_map[credit_id].text_color);
    end

end
