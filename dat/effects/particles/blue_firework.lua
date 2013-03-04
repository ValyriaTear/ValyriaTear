-- Blue fireworks
-- Author: roos, modifications by Bertram

systems = {}

systems[0] =
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
		radius=30,
		shape='CIRCLE',
		omnidirectional=true,
		orientation=0,
		outer_cone=0,
		inner_cone=0,
		initial_speed=200,
		initial_speed_variation=0,
		emission_rate=800,
		start_time=0,
		emitter_mode='looping',
		spin='COUNTERCLOCKWISE'
	},

	keyframes =
	{
		{  -- keyframe 1
			size_x=0.3,
			size_y=0.3,
			color={0,1,1,1},
			rotation_speed=0,
			size_variation_x=0,
			size_variation_y=0,
			rotation_speed_variation=0,
			color_variation={.7,.3,.3,0},
			time=0
		},

		{  -- keyframe 2
			size_x=0.6,
			size_y=0.6,
			color={0,0,1,0},
			rotation_speed=0,
			size_variation_x=0,
			size_variation_y=0,
			rotation_speed_variation=0,
			color_variation={.1,.3, .3,0},
			time=1.0
		}

	},

	animation_frames =
	{
		'img/effects/fire.png'
	},
	animation_frame_times =
	{
		16
	},
	enabled = true,
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
        wave_length = .5,
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
	rotation_used = true,
	rotate_to_velocity = true,
	speed_scale_used = true,
	speed_scale = 0.005,
	min_speed_scale = 1.0,
	max_speed_scale = 20.0,
	smooth_animation = false,
	modify_stencil = false,
	stencil_op = 'INCR',
	use_stencil = false,
	random_initial_angle = false
}
