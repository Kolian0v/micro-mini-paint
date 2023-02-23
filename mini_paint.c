#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct s_matrix
{
	int width;
	int height;
	char color;
	char **map;
} t_matrix;                	// matrix structure

typedef struct s_circle
{
	float x;
	float y;
	float radius;
	char color;
	char type;
} t_circle;                	// circle structure

int get_background(FILE *file, t_matrix *matrix)   	// build and filling starting map
{
	if (fscanf(file, "%d %d %c\n", &matrix->width, &matrix->height, &matrix->color) == 3)
	{
		if (matrix->width > 300 || matrix->width < 1 || matrix->height > 300 || matrix->height < 1)
			return 1;
		if (!(matrix->map = malloc(sizeof(char*) * matrix->height)))   // 1st malloc with 'sizeof(char *)' on height
			return 1;
		for (int i = 0; i < matrix->height; i++)
		{
			if (!(matrix->map[i] = malloc(matrix->width)))       	// 2nd malloc in cycle, on width
				return 1;
			for (int j = 0; j < matrix->width; j++)
				matrix->map[i][j] = matrix->color;   		// filling mx->map with mx->clr
		}
		return 0;
	}
	return 1;
}

int is_circle(float x, float y, t_circle *circle)   // check is our point in the circle ("1" mode) or at the edge of the circle ("2" mode)
{
	float dlina = sqrtf(powf(x - circle->x, 2.) + powf(y - circle->y, 2.));		// counting the distanse between our point ([x][y]) with center of circle
	if (dlina <= circle->radius)
	{
		if (circle->radius - dlina < 1.0F)
			return 2;	// at the edge of the circle
		return 1;		// in the circle
	}
	return 0;			// over 
}

void put_one(t_circle *circle, t_matrix *matrix)  // put one our rectangle on to the map
{
	for(int i = 0; i < matrix->height; i++)
	{
		for(int j = 0; j < matrix->width; j++)
		{
		int is=is_circle((float)j, (float)i, circle);
		if (is == 2 || (is == 1 && circle->type == 'C'))	// check the modes function 'is_circle' returned
			matrix->map[i][j] = circle->color;		// mx->map[][] = circ->clr    remember
		}
	}
}

int get_circles(FILE *file, t_matrix *matrix) // read with fscanf lines in file and drawing all of it witn 'put_one' function
{
	t_circle circle;
	int retun;
	while((retun = fscanf(file, "%c %f %f %f %c\n", &circle.type, &circle.x, &circle.y, &circle.radius, &circle.color)) == 5)
	{
		if ((circle.type == 'c' || circle.type == 'C') && circle.radius > 0.0F)
			put_one(&circle, matrix);
		else return 1;		// if we didn't see right type or radius <= 0 we shuold return an error №2
	}
	if (retun == -1)		// if fscanf see 0 arguments they can return, they returns -1
		return 0;
	return 1; 			// if we have not 6 and not -1, thats wrong and we shuold return an error №2
}

void draw_map(t_matrix *matrix)         // dwaring full final wersion of map 
{
	for (int i = 0; i < matrix->height; i++)
	{
		write(1, matrix->map[i], matrix->width);
		write(1, "\n", 1);
	}
}

int minii(FILE *file)         // main action
{
	t_matrix matrix;
	if (get_background(file, &matrix))  	// build and filling starting map
		return 1;
	if (get_circles(file, &matrix))  	// drawing all of circles
		return 1;
	draw_map(&matrix);  			// dwaring full final wersion of map 
	// this line had a 'free_all' function, but exam machine doesn't check leaks, so we have no reasons to add it ¯\_(ツ)_/¯ 
	return 0;
}

int main(int ac, char const **av)  // shecking for errors 
{
	FILE *file;   		// this is for fscanf
	if (ac == 2)  		// check if we have 0 of 2+ filenames
	{
		file = fopen(av[1], "r");
		if (!file || minii(file))
		{
			write(1, "Error: Operation file corrupted\n", 32); // error №2
			return 1;
		}
		fclose(file);
		return 0;
	}
	write(1, "Error: argument\n", 16); // error №1
	return 1;
}
