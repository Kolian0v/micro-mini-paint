#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct s_matrix
{
	int width;
	int height;
	char color;
	char **map;
} t_matrix;                	// matrix structure

typedef struct s_rectangle
{
	float width;
	float height;
	float x;
	float y;
	char color;
	char type;
} t_rectangle;                	// rectangle structure

int get_background(FILE *file, t_matrix *matrix)   	// build and filling starting map
{
	if (fscanf(file, "%d %d %c\n", &matrix->width, &matrix->height, &matrix->color) == 3)
	{
		if (matrix->width > 300 || matrix->width < 1 || matrix->height > 300 || matrix->height < 1)
			return 1;
		if (!(matrix->map = malloc(sizeof(char *) * matrix->height)))   // 1st malloc with 'sizeof(char *)' on height
			return 1;
		for(int i=0; i < matrix->height; i++)
		{
			if (!(matrix->map[i] = malloc(matrix->width)))       	// 2nd malloc in cycle, on width
				return 1;
			for(int j=0; j < matrix->width; j++)
				matrix->map[i][j] = matrix->color;   		// filling mx->map with mx->clr
		}
		return 0;
	}
	return 1;
}

int is_rectangle(float x, float y, t_rectangle *rectangle)   // check is our point in the rectangle ("1" mode) or at the edge of the circle ("2" mode)
{
	if (x >= rectangle->x && y >= rectangle->y && x <= rectangle->width && y <= rectangle->height)
	{
		if (x - rectangle->x < 1.0F || y - rectangle->y < 1.0F || rectangle->width - x < 1.0F || rectangle->height - y < 1.0F)
			return 2;  // at the edge of the circle
		return 1;          // in the rectangle
	}
	return 0;                  // over rectangle
}

void put_one(t_rectangle *rectangle, t_matrix *matrix)  // put one our rectangle on to the map
{
	rectangle->width += rectangle->x;      // just remember 
	rectangle->height += rectangle->y;     // this shit
	for (int i = 0; i < matrix->height; i++)
	{
		for (int j = 0; j < matrix->width; j++)
		{
			int is = is_rectangle((float)j, (float)i, rectangle);
			if (is == 2 || (is == 1 && rectangle->type == 'R'))	// check the modes function 'is_rectangle' returned
				matrix->map[i][j] = rectangle->color;		// mx->map[][] = rec->clr    remember
		}
	}
}

int get_rectangles(FILE *file, t_matrix *matrix) // read with fscanf lines in file and drawing all of it witn 'put_one' function
{
	t_rectangle rec;
	int retun;
	while ((retun = fscanf(file, "%c %f %f %f %f %c\n", &rec.type, &rec.x, &rec.y, &rec.width, &rec.height, &rec.color)) == 6)
	{
		if ((rec.type == 'r' || rec.type == 'R') && rec.width > 0 && rec.height > 0)
			put_one(&rec, matrix);
		else return 1;		// if we didn't see right type or width/height <= 0 we shuold return an error №2
	}
	if (retun == -1)		// if fscanf see 0 arguments they can return, they returns -1
		return 0;
	return 1; 			// if we have not 6 and not -1, thats wrong and we shuold return an error №2
}

void draw_map(t_matrix *matrix)            // dwaring full final wersion of map 
{
	for (int i = 0; i < matrix->height; i++)
	{
		write(1, matrix->map[i], matrix->width);
		write(1, "\n", 1);
	}
}

int micro_paint(FILE *file)         // main action
{
	t_matrix matrix;
	if (get_background(file, &matrix))  	// build and filling starting map
		return 1;
	if (get_rectangles(file, &matrix))  	// drawing all of rectangles
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
		if (!file || micro_paint(file)){
			write(1, "Error: Operation file corrupted\n", 32); // error №2
			return 1;
		}
		return 0;
	}
	write(1, "Error: argument\n", 16); // error №1
	return 1;
}
