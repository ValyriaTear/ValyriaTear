-- Save point particle effect
-- Author: Bertram

-- The effect overall size in pixel. Used to make the map mode draw the effect on time.
effect_collision_width = 6*16;
effect_collision_height = 7*16;

systems = {}

systems[0] =
{
	emitter =
	{
		x = 0,
		y = -64,
		x2 = 0, --useless when using a circle shape
		y2 = 0, --useless when using a circle shape
		center_x = 0,  --useless when using a circle shape
		center_y = 0,  --useless when using a circle shape
		x_variation = 0,
		y_variation = 0,
		radius = 70,
		shape = 'circle outline',
		omnidirectional = false,
		orientation = -1.6, --up
		outer_cone = 0.0,
		inner_cone = 0,
		initial_speed = 50, -- Change this when active
		initial_speed_variation = 15,
		emission_rate = 50, -- Change this when active save point
		start_time = 0,
		emitter_mode = 'looping',
		spin = 'clockwise' -- change this when active
	},

	keyframes =
	{
		{  -- keyframe 1
			size_x = 0.6,
			size_y = 0.62,
			color={0.5, 0.5, 1.0, 0.6},
			rotation_speed = 0.0,
			size_variation_x = 0.2,
			size_variation_y = 0.2,
			rotation_speed_variation = 0.0,
			color_variation = {0.3, 0.3, 0.0, 0.0},
			time = 0.0
		},

		{  -- keyframe 2
			size_x = 0.3,
			size_y = 0.3,
			color={0.5, 0.5, 1, 0.4},
			rotation_speed = 0.0,
			size_variation_x = 0.2,
			size_variation_y = 0.2,
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
	system_lifetime = 0.3,
	particle_lifetime = 1.8,
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
	attractor_falloff = 0.0,
	rotation_used = false,
	rotate_to_velocity = false,
	speed_scale_used = false,
	speed_scale = 0.005,
	min_speed_scale = 1.0,
	max_speed_scale = 20.0,
	smooth_animation = true,
	modify_stencil = false,
	stencil_op = 'INCR',
	use_stencil = false,
	scene_lighting = 0.6,
	random_initial_angle = true
}

