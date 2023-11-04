#include <stdio.h>

int main(int argc, const char **argv)
{
	if (argc != 3)
	{
		puts("Codemap Gen");
		puts("Generate codemap used by mule. Syntax of input.bin: 0x.? 0x.?, where the previous code is for dbcs, following for unicode.");
		puts("Usage: %s [input.txt] [output.bin]");
	}

	FILE *in = fopen(argv[1]);
	FILE *out = fopen(argv[2]);

	while (!feof(in))
	{
		
		if (2 == fscanf())
	}
}
