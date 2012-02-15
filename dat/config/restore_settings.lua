require "settings"

-- Sets all the settings back to their default values
function settings.SetDefaults ()
  settings.video_settings = settings.video_defaults;
  settings.audio_settings = settings.audio_defaults;
  settings.language_settings = settings.language_defaults;
  settings.key_settings = settings.key_defaults;
  settings.joystick_settings = settings.joystick_defaults;
end
