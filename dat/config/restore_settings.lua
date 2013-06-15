require "settings"

-- default settings file, use to restore default values when needed
settings_defaults = {}
settings_defaults.first_start = 1
settings_defaults.language_default = "en@quot"

settings_defaults.video_defaults = {}
settings_defaults.video_defaults.full_screen = false
settings_defaults.video_defaults.screen_resx = 800
settings_defaults.video_defaults.screen_resy = 600
settings_defaults.video_defaults.smooth_graphics = true
settings_defaults.video_settings.ui_theme = vt_system.Translate("Royal Silk")

settings_defaults.audio_defaults = {}
settings_defaults.audio_defaults.music_vol = 70
settings_defaults.audio_defaults.sound_vol = 80

settings_defaults.key_defaults = {}
settings_defaults.key_defaults.up = 273
settings_defaults.key_defaults.down = 274
settings_defaults.key_defaults.left = 276
settings_defaults.key_defaults.right = 275
settings_defaults.key_defaults.confirm = 102
settings_defaults.key_defaults.cancel = 100
settings_defaults.key_defaults.menu = 115
settings_defaults.key_defaults.pause = 32

settings_defaults.joystick_defaults = {}
settings_defaults.joystick_defaults.input_disabled = false
settings_defaults.joystick_defaults.x_axis = 0
settings_defaults.joystick_defaults.y_axis = 1
settings_defaults.joystick_defaults.threshold = 8192
settings_defaults.joystick_defaults.confirm = 0
settings_defaults.joystick_defaults.cancel = 1
settings_defaults.joystick_defaults.menu = 2
settings_defaults.joystick_defaults.pause = 6
settings_defaults.joystick_defaults.quit = 7

-- Not supported yet
settings_defaults.joystick_defaults.index = 0

-- Sets all the settings back to their default values
function settings.SetDefaults ()
  settings.video_settings = settings_defaults.video_defaults;
  settings.audio_settings = settings_defaults.audio_defaults;
  settings.language = settings_defaults.language_default;
  settings.key_settings = settings_defaults.key_defaults;
  settings.joystick_settings = settings_defaults.joystick_defaults;
end
