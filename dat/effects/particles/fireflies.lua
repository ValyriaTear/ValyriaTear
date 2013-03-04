-- Fireflies effect
-- Author: Bertram

-- The effect overall size in pixel. Used to make the map mode draw the effect on time.
effect_collision_width = 6*16;
effect_collision_height = 5*16;

systems = {}

systems[0] =
{
    emitter =
    {
        x = -32,
        y = -64,
        x2 = 32,
        y2 = -64,
        center_x = 0,
        center_y = 0,
        x_variation = 10,
        y_variation = 10,
        radius = 70,
        shape = 'line',
        omnidirectional = true,
        orientation = 1.6, -- omnidirectional, so don't care
        outer_cone = 0.0,
        inner_cone = 0,
        initial_speed = 10,
        initial_speed_variation = 0,
        emission_rate = 1,
        start_time = 0,
        emitter_mode = 'looping',
        spin = 'random'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.6,
            size_y = 0.6,
            color={0.8, 0.81, 0.22, 0.0},
            rotation_speed = 0.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 0.0
        },
        {  -- keyframe 2
            size_x = 0.6,
            size_y = 0.6,
            color={0.8, 0.81, 0.22, 0.6},
            rotation_speed = 0.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 0.5
        },

        {  -- keyframe 3
            size_x = 0.6,
            size_y = 0.6,
            color={0.8, 0.81, 0.22, 0.0},
            rotation_speed = 0.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'img/effects/outlined_circle_small.png'
    },
    animation_frame_times =
    {
        16
    },
    enabled = true,
    blend_mode = 13,
    system_lifetime = 3.0,
    particle_lifetime = 3.0,
    particle_lifetime_variation = 0.0,
    max_particles = 5,
    damping = 0.8,
    damping_variation = 0.2,
    acceleration_x = 0,
    acceleration_y = 0,
    acceleration_variation_x = 0,
    acceleration_variation_y = 0,
    wind_velocity_x = 0,
    wind_velocity_y = 0,
    wind_velocity_variation_x = 0,
    wind_velocity_variation_y = 0,
    wave_motion_used = true,
    wave_length = 1.0,
    wave_length_variation = 0,
    wave_amplitude = 5.0,
    wave_amplitude_variation = 1.0,
    tangential_acceleration = 0,
    tangential_acceleration_variation = 0,
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0.0,
    rotation_used = true,
    rotate_to_velocity = false,
    speed_scale_used = false,
    speed_scale = 0.005,
    min_speed_scale = 1.0,
    max_speed_scale = 20.0,
    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}
