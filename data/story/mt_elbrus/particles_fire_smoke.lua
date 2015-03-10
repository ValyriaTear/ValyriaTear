-- Village Fire particle
-- Author: roos, modifications by Bertram

-- The effect overall size in pixel. Used to make the map mode draw the effect on time
-- and when the character is going to collide with it.
map_effect_collision = {
    -- Collision rectangle
    effect_collision_width = 3 * 16,
    effect_collision_height = 3 * 16,
    -- Image rectangle
    effect_width = 6 * 16,
    effect_height = 6 * 16,
}

systems = {}
-- the fire
systems[0] =
{
    enabled = true,

    emitter =
    {
        x = -7,
        y = 0,
        x2 = 7,
        y2 = 0,
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 10,
        shape = 'line',
        omnidirectional = true,
        orientation = 0,
        angle_variation = 0.0,
        initial_speed = 0,
        initial_speed_variation = 0,
        emission_rate = 5,
        start_time = 0,
        emitter_mode = 'looping',
        spin = 'random'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 0.3,
            size_y = 0.3,
            color = {1, 1, 1, 0.8},
            rotation_speed = 1,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0.5,
            color_variation = {0, 0, 0, 0},
            time = 0
        },

        {  -- keyframe 2
            size_x = 0.2,
            size_y = 0.2,
            color = {1,1,1,0},
            rotation_speed = 1,
            size_variation_x = 0,
            size_variation_y = 0,
            rotation_speed_variation = 0.5,
            color_variation = {0, 0, 0, 0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/flame1.png',
        'data/particle_effects/flame2.png',
        'data/particle_effects/flame3.png',
        'data/particle_effects/flame4.png',
        'data/particle_effects/flame5.png',
        'data/particle_effects/flame6.png',
        'data/particle_effects/flame7.png',
        'data/particle_effects/flame8.png'
    },
    animation_frame_times =
    {
        128
    },

    blend_mode = 13,
    system_lifetime = 20,
    particle_lifetime = 20,
    particle_lifetime_variation = 0.00,
    max_particles = 100,
    damping = 1.0,
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

-- the smoke
systems[1] =
{
    enabled = true,

    emitter =
    {
        x = -7,
        y = -10,
        x2 = 7,
        y2 = -10, 
        center_x = 0,
        center_y = 0,
        x_variation = 0,
        y_variation = 0,
        radius = 70,
        shape = 'line',
        omnidirectional = false,
        orientation = -1.6, --up
        angle_variation = 0.3,
        initial_speed = 10,
        initial_speed_variation = 5,
        emission_rate = 4,
        start_time = 0,
        emitter_mode = 'looping',
        spin = 'clockwise'
    },

    -- Makes the particles rotate.
    rotation = {
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x = 2.0,
            size_y = 2.0,
            color={0.4, 0.4, 0.4, 0.6},
            rotation_speed = 0.2,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 0.0
        },

        {  -- keyframe 2
            size_x = 1.9,
            size_y = 1.9,
            color = {1.0, 1.0, 1.0, 0.0},
            rotation_speed = 0.6,
            size_variation_x = 0.0,
            size_variation_y = 0.0,
            rotation_speed_variation = 0.0,
            color_variation = {0.0, 0.0, 0.0, 0.0},
            time = 1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/dust.png'
    },
    animation_frame_times =
    {
        16
    },

    blend_mode = 13,
    system_lifetime = 5.8,
    particle_lifetime = 5.8,
    particle_lifetime_variation = 0.2,
    max_particles = 10,
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
