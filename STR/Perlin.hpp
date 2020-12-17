namespace Perlin
{
	float Perlin(float x, float y);
	float Perlin3(float x, float y, float z);
	float OctavePerlin(double x, double y, int octaves, double persistence);
	float OctavePerlin3(double x, double y, double z, int octaves, double persistence);
}