-- Earth circle particle effect
-- Author: Bertram

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x = 0,
        y = 0,
        x2 = 0, --useless when using a circle shape
        y2 = 0, --useless when using a circle shape
        center_x = 0,  --useless when using a circle shape
        center_y = 0,  --useless when using a circle shape
        x_variation = 0,
        y_variation = 0,
        radius = 80,
        shape = 'point',
        omnidirectional = false,
        orientation = -1.6, --up
        outer_cone = 0.0,
        inner_cone = 0,
        initial_speed = 0,
        initial_speed_variation = 0,
        emission_rate = 1,
        start_time = 0,
        emitter_mode = 'one shot',
        spin = 'counter clockwise'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 1.0,
            size_y = 0.6,
            color={0.0, 1.0, 0.0, 0.0},
            rotation_speed = 100.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.2, 0.2, 0.2, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 1.0,
            size_y = 0.6,
            color = {0.2, 1.0, 0.2, 1.0},
            rotation_speed = 2.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.2, 0.2, 0.2, 0.0},
            time = 0.1
        },

        {  -- keyframe 3
            size_x = 1.0,
            size_y = 0.6,
            color = {0.0, 1.0, 0.0, 0.0},
            rotation_speed = 2.0,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.2, 0.2, 0.2, 0.0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'img/effects/earth_circle.png'
    },
    animation_frame_times =
    {
        16
    },

    blend_mode = 13,
    system_lifetime = 4.0,
    particle_lifetime = 4.0,
    particle_lifetime_variation = 0.0,
    max_particles = 1,
    damping = 1,
    damping_variation = 0,
    acceleration_x = 0,
    acceleration_y = 0,
    acceleration_variation_x = 0,
    acceleration_variation_y = 0,
    wind_velocity_x = 0,
    wind_velocity_y = 0,
    wind_velocity_variation_x = 0,
    wind_velocity_variation_y = 0,

    tangential_acceleration = 0,
    tangential_acceleration_variation = 0,
    radial_acceleration = 200,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0.0,

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}
