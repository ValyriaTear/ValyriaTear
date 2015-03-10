-- Ultimate explosion
-- Author: roos, modifications by Bertram

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x=0,
        y=0,
        x2=0,
        y2=0,
        center_x=0,
        center_y=0,
        x_variation=0,
        y_variation=0,
        radius=200,
        shape='point',
        omnidirectional=false,
        orientation=-1.57,
        angle_variation = 0.0,
        initial_speed=0,
        initial_speed_variation=0,
        emission_rate=656,
        start_time=0,
        emitter_mode='BURST',
        spin='RANDOM'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x=0.3,
            size_y=0.3,
            color={1,1,1,1},
            rotation_speed=0,
            size_variation_x=0,
            size_variation_y=0,
            rotation_speed_variation=0,
            color_variation={0,0,0,0},
            time=0
        },

        {  -- keyframe 2
            size_x=10.1,
            size_y=10.3,
            color={1,1,1,1},
            rotation_speed=0,
            size_variation_x=0,
            size_variation_y=0,
            rotation_speed_variation=0,
            color_variation={0,0,0,0},
            time=1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/explosion.png'
    },
    animation_frame_times =
    {
        16
    },

    blend_mode = 13,
    system_lifetime = .3,
    particle_lifetime = 3.8,
    particle_lifetime_variation = 0.00,
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
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0,

    smooth_animation = false,
    modify_stencil = true,
    stencil_op = 'INCR',
    use_stencil = false,
    scene_lighting = 0.0,
    random_initial_angle = false
}


systems[1] =
{
    emitter =
    {
        x=0,
        y=0,
        x2=0,
        y2=0,
        center_x=0,
        center_y=0,
        x_variation=0,
        y_variation=0,
        radius=200,
        shape='point',
        omnidirectional=false,
        orientation=-1.57,
        outer_cone=0,
        inner_cone=0,
        initial_speed=0,
        initial_speed_variation=0,
        emission_rate=656,
        start_time=0,
        emitter_mode='BURST',
        spin='RANDOM'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x=0.3,
            size_y=0.3,
            color={.2,.3,1,0.5},
            rotation_speed=0,
            size_variation_x=0,
            size_variation_y=0,
            rotation_speed_variation=0,
            color_variation={0,0,0,0},
            time=0
        },

        {  -- keyframe 2
            size_x=10.1,
            size_y=10.3,
            color={.2,.3,1,0.5},
            rotation_speed=0,
            size_variation_x=0,
            size_variation_y=0,
            rotation_speed_variation=0,
            color_variation={0,0,0,0},
            time=1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/explosion.png'
    },
    animation_frame_times =
    {
        16
    },
    enabled = true,
    blend_mode = 12,
    system_lifetime = .3,
    particle_lifetime = 3.8,
    particle_lifetime_variation = 0.00,
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
    wave_motion_used = false,
    wave_length = .5,
    wave_length_variation = 0,
    wave_amplitude = 0,
    wave_amplitude_variation = 0,
    tangential_acceleration = 0,
    tangential_acceleration_variation = 0,
    radial_acceleration = 0,
    radial_acceleration_variation = 0,
    user_defined_attractor = false,
    attractor_falloff = 0,
    rotation_used = false,
    rotate_to_velocity = false,
    speed_scale_used = false,
    speed_scale = 0.005,
    min_speed_scale = 1.0,
    max_speed_scale = 20.0,
    smooth_animation = false,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    scene_lighting = 0.0,
    random_initial_angle = false
}

systems[2] =
{
    emitter =
    {
        x=-500,
        y=0,
        x2=500,
        y2=400,
        center_x=0,
        center_y=0,
        x_variation=0,
        y_variation=0,
        radius=200,
        shape='RECTANGLE',
        omnidirectional=false,
        orientation=-1.57,
        outer_cone=0,
        inner_cone=0,
        initial_speed=800,
        initial_speed_variation=230,
        emission_rate=656,
        start_time=0,
        emitter_mode='LOOPING',
        spin='RANDOM'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x=0.15,
            size_y=1.7,
            color={1,1,1,.6},
            rotation_speed=0,
            size_variation_x=0.0,
            size_variation_y=0.5,
            rotation_speed_variation=0,
            color_variation={.1,.1,0,.5},
            time=0
        },

        {  -- keyframe 2
            size_x=0.15,
            size_y=1.7,
            color={1,1,1,.4},
            rotation_speed=0,
            size_variation_x=0.0,
            size_variation_y=.6,
            rotation_speed_variation=0,
            color_variation={0,0,0,0},
            time=1.0
        }

    },

    animation_frames =
    {
        'data/particle_effects/outlined_circle_small.png'
    },
    animation_frame_times =
    {
        16
    },
    enabled = true,
    blend_mode = 13,
    system_lifetime = .3,
    particle_lifetime = 3.8,
    particle_lifetime_variation = 0.00,
    max_particles = 100000,
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
    attractor_falloff = 0,
    rotation_used = false,
    rotate_to_velocity = false,
    speed_scale_used = false,
    speed_scale = 0.005,
    min_speed_scale = 1.0,
    max_speed_scale = 20.0,
    smooth_animation = false,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = true,
    random_initial_angle = false
}
