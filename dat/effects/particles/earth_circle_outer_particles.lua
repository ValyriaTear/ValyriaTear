-- Earth circle particle effect
-- Author: Bertram

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x = 100,
        y = 60,
        x2 = 0,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 100,
        shape = 'ellipse outline',
        omnidirectional = false,
        orientation = -1.6, --up
        angle_variation = 0.0,
        initial_speed = 100,
        initial_speed_variation = 30,
        emission_rate = 1000,
        start_time = 2.8,
        emitter_mode = 'burst',
        spin = 'clockwise'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.6,
            size_y = 0.6,
            color={0.1, 1.0, 0.1, 0.6},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 0.3,
            size_y = 0.3,
            color = {1.0, 1.0, 1.0, 0.4},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
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

    blend_mode = 13,
    system_lifetime = 6.0,
    particle_lifetime = 4.8,
    particle_lifetime_variation = 0.0,
    max_particles = 1000,
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
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0.0,

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}

systems[1] =
{
    enabled = true,

    emitter =
    {
        x = 100,
        y = 60,
        x2 = 0,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 100,
        shape = 'ellipse outline',
        omnidirectional = false,
        orientation = -1.6, --up
        outer_cone = 0.0,
        inner_cone = 0,
        initial_speed = 100,
        initial_speed_variation = 30,
        emission_rate = 1000,
        start_time = 3.2,
        emitter_mode = 'burst',
        spin = 'clockwise'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.6,
            size_y = 0.6,
            color={0.1, 1.0, 0.1, 0.6},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 0.3,
            size_y = 0.3,
            color = {1.0, 1.0, 1.0, 0.4},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
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

    blend_mode = 13,
    system_lifetime = 6.0,
    particle_lifetime = 4.8,
    particle_lifetime_variation = 0.0,
    max_particles = 1000,
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
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0.0,

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}

systems[2] =
{
    enabled = true,

    emitter =
    {
        x = 100,
        y = 60,
        x2 = 0,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 100,
        shape = 'ellipse outline',
        omnidirectional = false,
        orientation = -1.6, --up
        outer_cone = 0.0,
        inner_cone = 0,
        initial_speed = 100,
        initial_speed_variation = 30,
        emission_rate = 1000,
        start_time = 3.6,
        emitter_mode = 'burst',
        spin = 'clockwise'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.6,
            size_y = 0.6,
            color={0.1, 1.0, 0.1, 0.6},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 0.3,
            size_y = 0.3,
            color = {1.0, 1.0, 1.0, 0.4},
            rotation_speed = 0.0,
            size_variation_x = 0.2,
            size_variation_y = 0.2,
            rotation_speed_variation = 0.0,
            color_variation = {0.3, 0.0, 0.3, 0.0},
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

    blend_mode = 13,
    system_lifetime = 6.0,
    particle_lifetime = 4.8,
    particle_lifetime_variation = 0.0,
    max_particles = 1000,
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
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0.0,

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = true
}