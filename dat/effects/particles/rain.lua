-- Rain particle effect
-- Author: roos, modifications by Bertram

-- Example of use:
-- <mode>:GetParticleManager():AddParticleEffect("dat/effects/particles/rain.lua", 512.0, 768.0);

systems = {}

systems[0] =
{
    enabled = true,

    emitter =
    {
        x=-612,
        y=-768,
        x2=512,
        y2=-768,
        center_x=0,
        center_y=0,
        x_variation=0,
        y_variation=0,
        radius=500,
        shape='line',
        omnidirectional=false,
        orientation=1.57,
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
            size_x=0.1,
            size_y=0.3,
            color={1,1,1,.15},
            rotation_speed=0,
            size_variation_x=.2,
            size_variation_y=.2,
            rotation_speed_variation=0,
            color_variation={.3,.3,0,0},
            time=0
        },

        {  -- keyframe 2
            size_x=0.1,
            size_y=0.3,
            color={1,1,1,.17},
            rotation_speed=0,
            size_variation_x=.2,
            size_variation_y=.2,
            rotation_speed_variation=0,
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
    wind_velocity_x = 100,
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
        -- rotate according to the 3d direction of the particle.
        rotate_to_velocity = {
            -- stretch the particle according to its speed.
            speed_scale = 0.005,
            min_speed_scale = 1.0,
            max_speed_scale = 20.0
        }
    },

    smooth_animation = true,
    modify_stencil = false,
    stencil_op = 'INCR',
    use_stencil = false,
    random_initial_angle = false
}
