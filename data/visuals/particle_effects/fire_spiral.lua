-- fire spiral
-- Author: roos, modifications by Bertram

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x = 0,
        y = 0,
        x2 = 0,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 30,
        shape = 'circle',
        omnidirectional = true,
        orientation = 0,
        angle_variation = 0.0,
        initial_speed = 200,
        initial_speed_variation = 0,
        emission_rate = 800,
        start_time = 0,
        emitter_mode = 'burst',
        spin = 'counterclockwise'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.3,
            size_y = 0.3,
            color = {1.0, 0.5, 1.0, 1.0},
            rotation_speed = 0,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0,
            color_variation = {0.7, 0.3, 0.3, 0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 0.6,
            size_y = 0.6,
            color = {1.0, 0.5, 0.0, 0.0},
            rotation_speed = 0,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0,
            color_variation = {0.1, 0.3, 0.3, 0.0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/fire.png'
    },
    animation_frame_times =
    {
        16
    },

    blend_mode = 13,
    system_lifetime = 3,
    particle_lifetime = 0.4,
    particle_lifetime_variation = 0.00,
    max_particles = 150,
    damping = 1,
    damping_variation = 0,
    acceleration_x = 0,
    acceleration_y = 2530,
    acceleration_variation_x = 0,
    acceleration_variation_y = 0,
    wind_velocity_x = 0,
    wind_velocity_y = 0,
    wind_velocity_variation_x = 0,
    wind_velocity_variation_y = 0,

    wave_motion = {
        wave_length = 0.5,
        wave_length_variation = 0,
        wave_amplitude = 0,
        wave_amplitude_variation = 0,
    },

    tangential_acceleration = 7880,
    tangential_acceleration_variation = 0,
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0,

    -- Makes the particles rotate.
    rotation = {
        -- rotate according to the 3d direction of the particle.
        rotate_to_velocity = {
            -- stretch the particle according to its speed.
            speed_scale = 0.005,
            min_speed_scale = 1.0,
            max_speed_scale = 20.0
        }
    },

    smooth_animation = false,
    modify_stencil = false,
    stencil_op = 'incr',
    use_stencil = false,
    random_initial_angle = false
}
