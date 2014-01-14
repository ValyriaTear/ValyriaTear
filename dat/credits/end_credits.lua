------------------------------------------------------------------------------[[
-- Filename: end_credits.lua
--
-- Description: Contains the credits seen at game ending.
-- It shall be moved from an episode end to another until reaching the actual game end.
------------------------------------------------------------------------------]]

local ns = {}
setmetatable(ns, {__index = _G})
end_credits = ns;
setfenv(1, ns);

local credit_map = {}
local credit_id = 0;
local credit_time = 0;
local credit_alpha = 1.0;

local dark_overlay = {};
local overlay_time = 0;

function Initialize(map_instance)
    Map = map_instance;

    Script = Map:GetScriptSupervisor();

    credit_map = {
        [0] = {
            images = nil,
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Thanks for playing!!"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 384.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [1] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/valyria_logo_black.png"),
                    image_dimension_x = 630.0,
                    image_dimension_y = 318.0,
                    image_position_x = 500.0,
                    image_position_y = 360.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Episode I"), vt_video.TextStyle("text28")),
                    text_position_x = 670.0,
                    text_position_y = 408.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [2] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/objects/paper_feather.lua"),
                    image_dimension_x = 60.0,
                    image_dimension_y = 56.0,
                    image_position_x = 400.0,
                    image_position_y = 300.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Story"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Bertram\n\nLilou", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [3] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/objects/dog1.lua"),
                    image_dimension_x = 76.0,
                    image_dimension_y = 84.0,
                    image_position_x = 620.0,
                    image_position_y = 300.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Lead Programming, Scripting, Maps"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Bertram", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [4] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/portraits/kalya_full.png"),
                    image_dimension_x = 182.0,
                    image_dimension_y = 350.0,
                    image_position_x = 300.0,
                    image_position_y = 580.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Additional programming and scripting"), vt_video.TextStyle("text28")),
                    text_position_x = 510.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Roots\n\nIkarusDowned\n\nCoderGreen\n\nAuthenticate\n\nLogzero\n\nEndoalir", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Balancing, ...\n\nQuest log, Minimap support, ...\n\nShop Trade mode support, ...\n\nUI theme change menu support, ...\n\nGL code improvement, ...\n\nMultiple screen fix, ..."), vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [5] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/battle/enemies/fenrir_0.lua"),
                    image_dimension_x = 560.0,
                    image_dimension_y = 406.0,
                    image_position_x = 300.0,
                    image_position_y = 350.0,
                },
                [1] = {
                    image = Script:CreateImage("img/logos/oga.png"),
                    image_dimension_x = 160.0,
                    image_dimension_y = 80.0,
                    image_position_x = 300.0,
                    image_position_y = 700.0,
                },
                [2] = {
                    image = Script:CreateAnimation("img/sprites/map/characters/bronann_frightened_unarmed_fixed.lua"),
                    image_dimension_x = 64.0,
                    image_dimension_y = 128.0,
                    image_position_x = 880.0,
                    image_position_y = 500.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Graphics"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Daniel Cook\n\nZabin\n\nSafir-Kreuz\n\nJetrel\n\nRamtam\n(vectorgurl.com)\n\nPatrick Thompson\n(pkillustration.com)\n\n"
                                            .."Open Game Art (OGA)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Village tileset, ...\n\nCemetery tileset, ...\n\nHeroes portraits, ...\n\nHeroes sprites, Cave tilesets, ...\n\n\nForest battle backgrounds, ...\n\n\nThe Fenrir\n\nA lot of various things"), vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [6] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/objects/layna_statue.lua"),
                    image_dimension_x = 60.0,
                    image_dimension_y = 170.0,
                    image_position_x = 300.0,
                    image_position_y = 250.0,
                },
                [1] = {
                    image = Script:CreateAnimation("img/sprites/battle/enemies/harlequin_0.lua"),
                    image_dimension_x = 79.0 * 2,
                    image_dimension_y = 94.0 * 2,
                    image_position_x = 170.0,
                    image_position_y = 420.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Graphics"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Redshrike (OGA)\n\neleazzar (OGA)\n\nBertram\n\nLilou\n\nRavenmore (OGA)\n\nKirill777 (OGA)\n\nAnthony44 (OGA)\n\nJAP (OGA)\n\nEcrivain (OGA)\n\nkillyoverdrive (OGA)",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 292.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText("qubodup (OGA)\n\nJordan Trudgett\njordan.trudgett.com\nOsmic (OGA)\n\nDelfos (OGA)\n\nLen (OGA)\n\nDaniel Eddeland (OGA)\n\nBart (OGA)\n\nSindwiller (OGA)\n\nhc (OGA)\n\nzookeeper (OGA)\n\nMrBeast (OGA)",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [3] = {
                    text = Script:CreateText("Sunburn (OGA)\n\nBlarumyrran (OGA)\n\nrichtaur (OGA)\n\nLamoot (OGA\n\nXenodora (OGA)\n\nThe Mana World (TMW)\n\nYuuki (TMW)\n\nPhilipp H. Poll\n\nTempest in the Aether\n\nUnknown Horizons",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 732.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [7] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/objects/cat1.lua"),
                    image_dimension_x = 22.0 * 2,
                    image_dimension_y = 25.0 * 2,
                    image_position_x = 400.0,
                    image_position_y = 220.0,
                },
                [1] = {
                    image = Script:CreateAnimation("img/sprites/map/objects/campfire.lua"),
                    image_dimension_x = 320.0 / 5 * 2,
                    image_dimension_y = 64.0 * 2,
                    image_position_x = 800.0,
                    image_position_y = 650.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Music"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Mathew Pablo\n(matthewpablo.com)\n\nZander Noriega (OGA)\n\nHorrorPen (OGA)\n\n"
                                             .."djsaryon (OGA)\n\nEdward J. Blakeley (OGA)\n\nKoertes (OGA)\n\nMumu (OGA)\n\nyd (OGA)",
                                             vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("\nMain theme, Village music, ...\n\nBanesore's battle, ...\n\nLayna Forest music, ...\n\nBoss Music\n\nBattle music\n\nBirds' song\n\nBattle lost music\n\nLayna cave music"), vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [8] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/characters/kalya_laughing.lua"),
                    image_dimension_x = 64.0,
                    image_dimension_y = 128.0,
                    image_position_x = 800.0,
                    image_position_y = 380.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Music"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("lasercheese (OGA)\n\nEliot Corley (el-corleo) (OGA)\n\nTom Peter (OGA)\n\n",
                                             vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Sad theme\n\nMountain music\n\nMountain Shrine music"), vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [9] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/characters/bronann_hero_stance_unarmed.lua"),
                    image_dimension_x = 96.0 / 3 * 2,
                    image_dimension_y = 64.0 * 2,
                    image_position_x = 200.0,
                    image_position_y = 280.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Sounds"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("qubodup (OGA)\n\nBart (OGA)\n\nBertram\n\nMichel Bardari (OGA)\n\nartisticdude (OGA)\n\n"..
                                             "Blender Foundation\napricot.blender.org",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 312.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText("lendrick (freesound)\n\nSpandau (freesound)\n\nTheGertz (freesound)\n\nEcrivain(OGA)\n\nBrandon Morris (OGA)\n\nIndependent.nu (OGA)",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [3] = {
                    text = Script:CreateText("DoKashiteru (OGA)\n\nNick Bowler (OGA)\n\nkddekadenz (OGA)\n\nremaxim (OGA)\n\nj1987\n\nOveMelaa (OGA)",
                                             vt_video.TextStyle("text20")),
                    text_position_x = 712.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [10] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/map/characters/bronann_bed_animation.lua"),
                    image_dimension_x = 168.0 / 3 * 2,
                    image_dimension_y = 273.0 /3 * 2,
                    image_position_x = 820.0,
                    image_position_y = 580.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Ports"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Ablu\n\nSocapex\n\nIbara\n\nAMDmi3\n\nMCMic", vt_video.TextStyle("text24")),
                    text_position_x = 312.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText("Fedora\n\nOSX\n\nOpenBSD\n\nFreeBSD\n\nArchLinux", vt_video.TextStyle("text24")),
                    text_position_x = 382.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
                [3] = {
                    text = Script:CreateText("hasufell\n\nakien-Mageia\n\nAapoRantalainen\n\nptitSeb\n\nEric Anholt", vt_video.TextStyle("text24")),
                    text_position_x = 612.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [4] = {
                    text = Script:CreateText("Gentoo\n\nMageia\n\nMaemo\n\nOpen Pandora\n\nDebian", vt_video.TextStyle("text24")),
                    text_position_x = 682.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [11] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/sprites/map/characters/bronann_hurt_west.png"),
                    image_dimension_x = 128.0,
                    image_dimension_y = 128.0,
                    image_position_x = 220.0,
                    image_position_y = 500.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Translations"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("mmstick, Astralchaos\n\nBertram\n\nGallaecio\n\nNaN\n\nBioHazardX\n\nKnitter\n\ndimproject\n\njahalic", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("English text review\n\nFrench (fr)\n\nGallician (gl)\n\nGerman (de)\n\nItalian (it)\n\nPortuguese (pt_PT)\n\nRussian (ru)\n\nSpanish (es)"), vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [12] = {
            images = {
                [0] = {
                    image = Script:CreateAnimation("img/sprites/battle/characters/kalya/kalya_victory.lua"),
                    image_dimension_x = 256.0 / 4 * 2,
                    image_dimension_y = 64.0 * 2,
                    image_position_x = 650.0,
                    image_position_y = 320.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Testers"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Shirish\n\nKhyana\n\nSahaondra\n\nOmega\n\nPenPen", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [13] = {
            images = nil,
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("And you..."), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 384.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [14] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/hoa_garland_logo.png"),
                    image_dimension_x = 412.0,
                    image_dimension_y = 200.0,
                    image_position_x = 512.0,
                    image_position_y = 330.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Valyria Tear was forked from Hero of Allacrost in 2011"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 384.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
            },
        },
        [15] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Development Team (2004-2011)\n\nFounder and Lead Designer"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Tyler Olsen (Roots)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [16] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Project Leads/Administrators"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Tyler Olsen (Roots)\n\nJacob Rudolph (rujasu)\n\nPhilip Vorsilak (gorzuate)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
        [17] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Programming Team (2004-2011)"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Andy Gardner (ChopperDave)\n\n" ..
                                             "Baris Soner Usakli (Black Knight)\n\n" ..
                                             "Daniel Steuernol (Steu)\n\n" ..
                                             "Guillaume Anctil (Drakkoon)\n\n" ..
                                             "Jacob Rudolph (rujasu)\n\n" ..
                                             "Lindsay Roberts (Linds)\n\n" ..
                                             "Moises Ferrer Serra (byaku)\n\n" ..
                                             "Philip Vorsilak (gorzuate)\n\n" ..
                                             "Tyler Olsen (Roots)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Battle code, Menu code\n\nGame editor\n\nGame engine\n\nMap code\n\nVarious game modes, content scripting\n\nVideo engine\n\nVideo engine, Audio engine\n\nGame editor\n\nProgramming lead, Game engine, Map code"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [18] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Programming Team"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Viljami Korhonen (MindFlayer)\n\n" ..
                                             "Wouter Lindenhof (ElonNarai)\n\n" ..
                                             "Aaron Smith (Etherstar)\n\n" ..
                                             "Alastair Lynn (prophile)\n\n" ..
                                             "Brandon Barnes (Winter Knight)\n\n" ..
                                             "Corey Hoffstein (visage)\n\n" ..
                                             "Dale Ma (eguitarz)\n\n" ..
                                             "DongHa Lee (MoOshiCow)\n\n" ..
                                             "Farooq Mela (CamelJockey)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Boot code, Battle code\n\nGame editor\n\nAudio engine\n\nVarious code, Mac OS X maintenance\n\nGame engine, Game modes, Windows build scripts\n\nBattle code\n\nMap editor\n\nInternationalization\n\nVideo engine"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [19] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Programming Team"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Kevin Martin (kev82)\n\n" ..
                                             "Nick Weihs (nickw)\n\n" ..
                                             "Raj Sharma (roos)\n\n" ..
                                             "Vladimir Mitrovic (snipe714)\n\n" ..
                                             "(densuke)\n\n" ..
                                             "Andreas Pawlak (nemesis)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Video engine, Game engine\n\nVideo engine\n\nVideo engine\n\nScripting engine\n\nVarious code\n\nVarious code"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [20] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Artwork Team (2004-2011)"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Brett Steele (Safir-Kreuz)\n\n" ..
                                             "Joe Raucci (Sylon)\n\n" ..
                                             "Josiah Tobin (Josiah Tobin)\n\n" ..
                                             "Matthew James (nunvuru)\n\n" ..
                                             "Richard Kettering (Jetryl)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Map sprites, Battle sprites, Character portraits, Location graphics\n\nBattle sprites, old Allacrost GUI artwork\n\nMap tiles\n\nAllacrost website graphics, GUI artwork, Game logos\n\nMap tiles, map sprites, Inventory icons, Allacrost artwork coordination"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [21] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Adam Black (indigoshift)\n\n" ..
                                             "Blake Heatly (KaelisEbonrai)\n\n" ..
                                             "Chris Hopp (fydo)\n\n" ..
                                             "Chris Luspo (Venndetta1)\n\n" ..
                                             "Francisco Munoz (fmunoz)\n\n" ..
                                             "Jason Frailey (Valdroni)\n\n" ..
                                             "Jerimiah Short (BigPapaN0z)\n\n" ..
                                             "John W. Bjerk (eleazar)\n\n" ..
                                             "Jon Williams (Jonatron)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Concept art\n\nInventory icons\n\nMap sprite animations\n\nConcept art\n\nInventory icons\n\nConcept art, Map sprites\n\nMap tiles\n\nMap tiles\n\nMap sprites"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [22] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Herbert Glaser (hollowgrind)\n\n" ..
                                             "Mark Goodenough (RangerM)\n\n" ..
                                             "Max Humber (zomby138)\n\n" ..
                                             "Nathan Christie (Adarias)\n\n" ..
                                             "Peter Geinitz (wayfarer)\n\n" ..
                                             "Richard Marks (DeveloperX)\n\n", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Map sprite animations\n\nMap sprite animations, Map tiles\n\nConcept art, Allacrost title screen image\n\nConcept art, Map sprites, Map tiles\n\nConcept art, Map sprites\n\nMap tiles"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [23] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Artwork Team"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                    text = Script:CreateText("Tyler Olsen (Roots)\n\n" ..
                                             "Tyler Stroud (gloomcover)\n\n" ..
                                             "Vicki Beinhart (Namakoro)\n\n" ..
                                             "Victoria Smith (alenacat)\n\n" ..
                                             "(Jarks)\n\n" ..
                                             "(Psiweapon)", vt_video.TextStyle("text24")),
                    text_position_x = 512.0,
                    text_position_y = 300.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Various art\n\nMap tiles\n\nEnemy sprites\n\nMap sprites, Map tiles\n\nMap tiles, Inventory icons\n\nMap tiles, Battle sprites"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [24] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Music and Sound Team (2004-2011)"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                text = Script:CreateText("Jamie Bremaneson (Jam)\n\n" ..
                                         "Joe Rouse (Loodwig)\n\n" ..
                                         "Ryan Reilly (Rain)\n\n" ..
                                         "Samuel Justice (sam_justice)\n\n" ..
                                         "Zhe Zhou (shizeet)\n\n" ..
                                         "Jean Malary (hamiko)\n\n" ..
                                         "Matt Dexter (Star Pilot)", vt_video.TextStyle("text24")),
                        text_position_x = 512.0,
                        text_position_y = 300.0,
                        text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Sound composer\n\nSoundtrack composer\n\nMusic and sound lead, Soundtrack composer\n\nSound composer\n\nSound composer\n\nSound mixer\n\nSoundtrack composer"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [25] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/logos/main_logo_sword.png"),
                    image_dimension_x = 130.0,
                    image_dimension_y = 282.0,
                    image_position_x = 212.0,
                    image_position_y = 600.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("Allacrost Special Thanks"), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 200.0,
                    text_color = vt_video.Color(1.0, 0.8, 0.7, 1.0),
                },
                [1] = {
                        text = Script:CreateText("Daniel Cook (Danc)\n\n" ..
                                                 "The Battle for Wesnoth Development Team\n\n" ..
                                                 "(neoriceisgood)\n\n" ..
                                                 "(Melchior)\n\n" ..
                                                 "(Egan1)", vt_video.TextStyle("text24")),
                        text_position_x = 512.0,
                        text_position_y = 300.0,
                        text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
                [2] = {
                    text = Script:CreateText(vt_system.Translate("Map Tiles\n\nInventory Icons\n\nMap monster sprites"),
                                             vt_video.TextStyle("text24")),
                    text_position_x = 642.0,
                    text_position_y = 324.0,
                    text_color = vt_video.Color(0.7, 0.7, 0.7, 1.0),
                },
            },
        },
        [26] = {
            images = {
                [0] = {
                    image = Script:CreateImage("img/backdrops/boot/crystal.png"),
                    image_dimension_x = 140.0,
                    image_dimension_y = 220.0,
                    image_position_x = 512.0,
                    image_position_y = 400.0,
                },
                [1] = {
                    image = Script:CreateImage("img/backdrops/boot/flare.png"),
                    image_dimension_x = 256.0,
                    image_dimension_y = 256.0,
                    image_position_x = 512.0,
                    image_position_y = 400.0,
                },
            },
            texts = {
                [0] = {
                    text = Script:CreateText(vt_system.Translate("To be continued..."), vt_video.TextStyle("text28")),
                    text_position_x = 512.0,
                    text_position_y = 584.0,
                    text_color = vt_video.Color(1.0, 1.0, 1.0, 1.0),
                },
            },
        },
    }

    credit_id = 0;
    credit_time = 0;
    set_image_dimensions = false;

    dark_overlay = Script:CreateImage("img/ambient/black.png");
    dark_overlay:SetDimensions(1024.0, 768.0);
    overlay_time = 0;

    -- Set the requested dimensions of the first image
    if (credit_map[credit_id] ~= nil and credit_map[credit_id].images ~= nil) then
        for id, _ in pairs(credit_map[credit_id].images) do
            local _images = credit_map[credit_id].images[id];
            if (_images.image ~= nil) then
                _images.image:SetDimensions(_images.image_dimension_x, _images.image_dimension_y);
            end
        end
    end
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
        -- Set the requested dimensions of each images once.
        if (credit_map[credit_id] ~= nil and credit_map[credit_id].images ~= nil) then
            for id, _ in pairs(credit_map[credit_id].images) do
                local _images = credit_map[credit_id].images[id];
                if (_images.image ~= nil) then
                    _images.image:SetDimensions(_images.image_dimension_x, _images.image_dimension_y);
                end
            end
        end

    end

    -- Updates potential animations
    if (credit_map[credit_id] ~= nil and credit_map[credit_id].images ~= nil) then
        for id, _ in pairs(credit_map[credit_id].images) do
            local _images = credit_map[credit_id].images[id];
            if (_images.image ~= nil) then
                _images.image:Update();
            end
        end
    end

    -- Update the current credit display time.
    if (credit_time >= 0
            and credit_time <= 1000) then
        credit_alpha = credit_time / 1000;
    elseif (credit_time > 6000
            and credit_time <= 7000) then
        credit_alpha = 1.0 - (credit_time - 6000) / (7000 - 6000);
    elseif (credit_time > 7000) then
        credit_alpha = 0.0;
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

    -- Images
    if (credit_map[credit_id].images ~= nil) then
        -- For each image
        for id, _ in pairs(credit_map[credit_id].images) do
            local _images = credit_map[credit_id].images[id];
            if (_images.image ~= nil) then
                image_color:SetAlpha(0.8 * credit_alpha);
                VideoManager:Move(_images.image_position_x, _images.image_position_y);
                _images.image:Draw(image_color);
            end
        end -- for
    end
    -- Texts
    if (credit_map[credit_id].texts ~= nil) then
        -- For each piece of text
        for id, _ in pairs(credit_map[credit_id].texts) do
            local _texts = credit_map[credit_id].texts[id];
            if (_texts.text ~= nil) then
                _texts.text_color:SetAlpha(0.9 * credit_alpha);
                VideoManager:Move(_texts.text_position_x, _texts.text_position_y);
                _texts.text:Draw(_texts.text_color);
            end
        end -- for
    end

end
