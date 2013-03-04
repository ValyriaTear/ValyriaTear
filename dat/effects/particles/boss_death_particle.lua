-- Boss death particle effect

systems = {}

systems[0] =
{
    emitter =
    {
        x=-64,
        y=-64,
        x2=64,
        y2=64,
        center_x=0,
        center_y=0,
        x_variation=0,
        y_variation=0,
        radius=500,
        shape='circle',
        omnidirectional=false,
        orientation=1.57,
        outer_cone=0,
        inner_cone=0,
        initial_speed=200,
        initial_speed_variation=50,
        emission_rate=156,
        start_time=0,
        emitter_mode='one shot',
        spin='RANDOM'
    },

    keyframes =
    {
        {  -- keyframe 1
            size_x=0.3,
            size_y=0.3,
            color={1,1,1,.6},
            rotation_speed=1,
            size_variation_x=.2,
            size_variation_y=.2,
            rotation_speed_variation=.5,
            color_variation={.3,.3,0,0},
            time=0
        },

        {  -- keyframe 2
            size_x=0.3,
            size_y=0.3,
            color={1,1,1,.4},
            rotation_speed=1,
            size_variation_x=.2,
            size_variation_y=.2,
            rotation_speed_variation=.5,
            color_variation={0,0,0,0},
            time=1.0
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
    system_lifetime = .6,
    particle_lifetime = 6.8,
    particle_lifetime_variation = 0.00,
    max_particles = 1000,
    damping = 1,
    damping_variation = 0,
    acceleration_x = 0,
    acceleration_y = 0,
    acceleration_variation_x = 0,
    acceleration_variation_y = 0,
    wind_velocity_x = 70,
    wind_velocity_y = 0,
    wind_velocity_variation_x = 60,
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

