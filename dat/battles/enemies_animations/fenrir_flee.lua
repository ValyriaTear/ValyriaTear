-- Set the namespace
local ns = {};
setmetatable(ns, {__index = _G});
fenrir_flee = ns;
setfenv(1, ns);

-- local references
local enemy = {};
local enemy_pos_x = 0.0;

local distance_moved_x = 0.0;

local Battle = {};

-- battle, the Game mode
-- _enemy, the BattleEnemy dying
function Initialize(battle, _enemy)
    -- Keep the reference in memory
    Battle = battle;
    enemy = _enemy;
end


function Update()

    enemy_pos_x = enemy:GetXLocation();

    -- Return once the Fenrir is out of the screen.
    if (enemy_pos_x > 900.0) then
        return true;
    end

    distance_moved_x = SystemManager:GetUpdateTime() * 0.20;
    enemy_pos_x = enemy_pos_x + distance_moved_x;
    enemy:SetXLocation(enemy_pos_x);

    -- Also fade out the enemy
    enemy:SetSpriteAlpha(enemy:GetSpriteAlpha() - (distance_moved_x / 200.0));
    return false;
end
