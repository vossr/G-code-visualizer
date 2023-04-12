static void	gcode_parse_op_lerp(char *line, vec3 *pos)
{
	u32 i = 0;

	float scale = 0.001;//mm to meters
	scale *= 5;
	while (line[i])
	{
		if (line[i] == 'X')
			pos->x = atof(&line[i + 1]) * scale;
		else if (line[i] == 'Y')
			pos->z = atof(&line[i + 1]) * scale;
		else if (line[i] == 'Z')
			pos->y = atof(&line[i + 1]) * scale;
		i++;
	}
}

static void	parse_gcode_path(t_app *app, char **file)
{
	u32 i = 0;
	vec3 current_pos = VEC_ZERO;

	while (file[i])
	{
		if (string_starts_with(file[i], "G0")
			|| string_starts_with(file[i], "G1"))
		{
			gcode_parse_op_lerp(file[i], &current_pos);
			vector_push(&app->tmp_path, current_pos);
		}
		i++;
	}
}

static void	center_path(t_app *app)
{
	vec3 avg = VEC_ZERO;
	vector_loop(app->tmp_path, path)
		avg = vec_avg(avg, path[i]);
	avg.y /= 2.0;
	vector_loop(app->tmp_path, path)
		vec_sub_this(&path[i], avg);
}

static void	parse_gcode(t_app *app, char **file)
{
	vector_scale(&app->tmp_path, 0);
	parse_gcode_path(app, file);

	if (app->tmp_path.current_size < 20)
	{
		printf("Unable to read gcode from file\n");
		return;
	}
	//clean path ends for visual purposes
	for (int i = 0; i < 10; i++)
		vector_delete_middle(&app->tmp_path, 0);
	for (int i = 0; i < 5; i++)
		vector_delete_last(&app->tmp_path);
	center_path(app);
	vector_copy(&app->tmp_path, &app->path);
}
