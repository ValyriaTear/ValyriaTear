------------------------------------------------------------------------------[[
-- Filename: small_credits.lua
--
-- Description: Contains the credits seen at game start, only the main
-- authors are there.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
episode1_credits = ns;
setfenv(1, ns);

local open_game_art_logo_id = 0;
local allacrost_logo_id = 0;

local current_text_header = "";
local current_text = "";

local credit_map = {}
local credit_id = 0;
local credit_time = 0;

local intro_time_done = false;

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    allacrost_logo_id = Script:AddImage("img/logos/hoa_garland_logo.png", 206.0, 100.0);
    open_game_art_logo_id = Script:AddImage("img/logos/oga.png", 160.0, 80.0);

    credit_map = {
        [0] = {
            header = vt_system.Translate("Based on the engine"),
            text = ""
        },
        [1] = {
            header = vt_system.Translate("Story"),
            text = "Bertram\nLilou"
        },
        [2] = {
            header = vt_system.Translate("Episode I - Main Graphics"),
            text = "Daniel Cook\nZabin\nSafir-Kreuz\nJetrel"
        },
        [3] = {
            header = vt_system.Translate("Episode I - Additional Graphics"),
            text = "Open Game Art\nRamtam\n(vectorgurl.com)\nPatrick Thompson\n(pkillustration.com)\nand many others"
        },
        [4] = {
            header = vt_system.Translate("Music"),
            text = "Mathew Pablo\n(matthewpablo.com)\nZander Noriega (OGA)\nKoertes (OGA)\nHorrorPen (OGA)"
        },
        [5] = {
            header = vt_system.Translate("Programming, Scripting, Maps"),
            text = "Bertram"
        },
        [6] = {
            header = vt_system.Translate("Additional programming\nand scripting"),
            text = "Roots\nIkarusDowned\nCoderGreen"
        },
        [7] = {
            header = vt_system.Translate("Translations"),
            text = "NaN (DE)\nBioHazardX (IT)\nKnitter (pt_PT)\nBertram (FR)"
        },
        [8] = {
            header = vt_system.Translate("Tester Team"),
            text = "Shirish\nKhyana\nSahaondra\nOmega\nPenPen"
        },
        [9] = {
            header = "",
            text = vt_system.Translate("To Lilou and Léo...")
        },
        [10] = {
            header = "",
            text = vt_system.Translate("Thanks to everyone\nwho made this possible!!")
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

    VideoManager:DrawText(credit_map[credit_id].header, 870.0, 600.0, vt_video.Color(1.0, 0.8, 0.7, 1.0 * text_alpha));
    VideoManager:DrawText(credit_map[credit_id].text, 870.0, 648.0, vt_video.Color(1.0, 1.0, 1.0, 0.9 * text_alpha));

    -- Custom images in honor of those who helped :)
    if (credit_id == 0) then
        Script:DrawImage(allacrost_logo_id, 870.0, 720.0, vt_video.Color(1.0, 1.0, 1.0, 0.8 * text_alpha));
    elseif (credit_id == 3) then
        Script:DrawImage(open_game_art_logo_id, 690.0, 710.0, vt_video.Color(1.0, 1.0, 1.0, 0.8 * text_alpha));
    end

end
