-- Dust particle effect
-- Author: roos, modifications by Bertram

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x = 0,
        y = -20,
        x2 = 0,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 30,
        shape = 'circle',
        omnidirectional = true,
        orientation = 0, -- 1.6
        outer_cone = 0,
        inner_cone = 0,
        initial_speed = 200,
        initial_speed_variation = 0,
        emission_rate = 1,
        start_time = 0,
        emitter_mode = 'burst',
        spin = 'random'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.9,
            size_y = 0.9,
            color = {0.4, 0.4, 0.4, 0.2},
            rotation_speed = 1,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0.5,
            color_variation={0.0, 0.0, 0.0, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 0.8,
            size_y = 0.8,
            color={0.3, 0.3, 0.3, 0.0},
            rotation_speed = 1,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0.5,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'img/effects/dust.png'
    },
    animation_frame_times =
    {
        16
    },

    blend_mode = 13,
    system_lifetime = 3,
    particle_lifetime = 0.4,
    particle_lifetime_variation = 0.00,
    max_particles = 1,
    damping = 0.01,
    damping_variation = 0,
    acceleration_x = 0,
    acceleration_y = 0,
    acceleration_variation_x = 0,
    acceleration_variation_y = 0,
    wind_velocity_x = 0,
    wind_velocity_y = -155,
    wind_velocity_variation_x = 0,
    wind_velocity_variation_y = 0,

    tangential_acceleration = 0,
    tangential_acceleration_variation = 0,
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0,

    -- Makes the particles rotate.
    rotation = {
    },

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}
