------------------------------------------------------------------------------[[
-- Filename: episode1_credits.lua
--
-- Description: Contains the credits seen at game start, only the main
-- authors are there.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
episode1_credits = ns;
setfenv(1, ns);

local open_game_art_logo = {};
local allacrost_logo = {};

local credit_map = {}
local credit_id = 0;
local credit_time = 0;

local intro_time_done = false;

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    allacrost_logo = Script:CreateImage("img/logos/hoa_garland_logo.png");
    allacrost_logo:SetDimensions(206.0, 100.0);
    open_game_art_logo = Script:CreateImage("img/logos/oga.png");
    open_game_art_logo:SetDimensions(160.0, 80.0);

    credit_map = {
        [0] = {
            header = Script:CreateText(vt_system.Translate("Based on the engine"), vt_video.TextStyle("text22")),
            text = Script:CreateText("", vt_video.TextStyle("text22"))
        },
        [1] = {
            header = Script:CreateText(vt_system.Translate("Story"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Bertram\nLilou", vt_video.TextStyle("text22")),
        },
        [2] = {
            header = Script:CreateText(vt_system.Translate("Episode I - Main Graphics"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Daniel Cook\nZabin\nSafir-Kreuz\nJetrel", vt_video.TextStyle("text22")),
        },
        [3] = {
            header = Script:CreateText(vt_system.Translate("Episode I - Additional Graphics"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Open Game Art\nRamtam\n(vectorgurl.com)\nPatrick Thompson\n(pkillustration.com)\n"..vt_system.Translate("and many others"),
                                     vt_video.TextStyle("text22")),
        },
        [4] = {
            header = Script:CreateText(vt_system.Translate("Music"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Mathew Pablo\n(matthewpablo.com)\nZander Noriega (OGA)\nKoertes (OGA)\nHorrorPen (OGA)",
                                     vt_video.TextStyle("text22")),
        },
        [5] = {
            header = Script:CreateText(vt_system.Translate("Programming, Scripting, Maps"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Bertram", vt_video.TextStyle("text22"))
        },
        [6] = {
            header = Script:CreateText(vt_system.Translate("Additional programming\nand scripting"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Roots\nIkarusDowned   CoderGreen\nAuthenticate   Logzero\nEndoalir", vt_video.TextStyle("text22"))
        },
        [7] = {
            header = Script:CreateText(vt_system.Translate("Ports"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Ablu\nSocapex\nIbara\nAMDmi3\nptitSeb", vt_video.TextStyle("text22"))
        },
        [8] = {
            header = Script:CreateText(vt_system.Translate("Translations"), vt_video.TextStyle("text22")),
            text = Script:CreateText("NaN (de)\nGallaecio (gl)\nBioHazardX (it)\nKnitter (pt_PT)", vt_video.TextStyle("text22"))
        },
        [9] = {
            header = Script:CreateText(vt_system.Translate("Translations"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Bertram (fr)\ndimproject (ru)\njahalic (es)\nWillemS (nl_NL)\nalexanderkjall (sv)", vt_video.TextStyle("text22"))
        },
        [10] = {
            header = Script:CreateText(vt_system.Translate("Tester Team"), vt_video.TextStyle("text22")),
            text = Script:CreateText("Shirish\nKhyana\nSahaondra\nOmega\nPenPen", vt_video.TextStyle("text22"))
        },
        [11] = {
            header = Script:CreateText("", vt_video.TextStyle("text22")),
            text = Script:CreateText(vt_system.Translate("To Lilou and Léo..."), vt_video.TextStyle("text22"))
        },
        [12] = {
            header = Script:CreateText("", vt_video.TextStyle("text22")),
            text = Script:CreateText(vt_system.Translate("Thanks to everyone\nwho made this possible!!"), vt_video.TextStyle("text22"))
        }
    }

    -- Get back where the credits were before changing map if it was the case.
    -- We also count the fact that the latest credit shown on the former map is seen, thus the +1 here.
    if (GlobalManager:DoesEventExist("game", "credit_id") == true) then
        credit_id = GlobalManager:GetEventValue("game", "credit_id") + 1;
    end

    -- Remove the intro time once done
    if (GlobalManager:DoesEventExist("game", "credit_intro_wait_time_done")) then
        intro_time_done = true;
    end
end

function Update()
    if (GlobalManager:DoesEventExist("game", "Start_Credits") == false) then
        return;
    end

    if (GlobalManager:DoesEventExist("game", "Credit_shown") == true) then
        return;
    end

    -- Stop once the credits have finished.
    if (credit_map[credit_id] == nil) then
        return;
    end

    local time_expired = SystemManager:GetUpdateTime();

    -- Handle the credit timer
    credit_time = credit_time + time_expired;

    -- Start the timer
    if (credit_time > 5000) then
        credit_time = 0

        -- Wait 5 seconds before starting the credits.
        if (intro_time_done == false) then
            intro_time_done = true;
            GlobalManager:SetEventValue("game", "credit_intro_wait_time_done", 1);
            return;
        end

        credit_id = credit_id + 1;
        -- Make the game aware of the current credit to shown to avoid making them restart on each map change.
        GlobalManager:SetEventValue("game", "credit_id", credit_id);


        -- Test whether the credits ended
        if (credit_map[credit_id] == nil) then
            GlobalManager:SetEventValue("game", "Credit_shown", 1);
        end
    end

end

local header_color = vt_video.Color(1.0, 0.8, 0.7, 1.0);
local text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);
local logo_color = vt_video.Color(1.0, 1.0, 1.0, 1.0);

function DrawPostEffects()
    -- Stop once the credits have finished.
    if (credit_map[credit_id] == nil or intro_time_done == false) then
        return;
    end

    if (GlobalManager:DoesEventExist("game", "Start_Credits") == false) then
        return;
    end

    if (GlobalManager:DoesEventExist("game", "Credit_shown") == true) then
        return;
    end

    local text_alpha = 1.0;
    if (credit_time >= 0
            and credit_time <= 1000) then
        text_alpha = credit_time / 1000;
    elseif (credit_time > 4000
            and credit_time <= 5000) then
        text_alpha = 1.0 - (credit_time - 4000) / (5000 - 4000);
    elseif (credit_time > 5000) then
        text_alpha = 0.0;
    end

    header_color:SetAlpha(1.0 * text_alpha);
    text_color:SetAlpha(0.9 * text_alpha);
    VideoManager:Move(870.0, 600.0);
    credit_map[credit_id].header:Draw(header_color);
    VideoManager:Move(870.0, 648.0);
    credit_map[credit_id].text:Draw(text_color);

    -- Custom images in honor of those who helped :)
    logo_color:SetAlpha(0.8 * text_alpha);
    if (credit_id == 0) then
        VideoManager:Move(870.0, 720.0);
        allacrost_logo:Draw(logo_color);
    elseif (credit_id == 3) then
        VideoManager:Move(690.0, 710.0);
        open_game_art_logo:Draw(logo_color);
    end

end
