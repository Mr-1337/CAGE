#include "Car.hpp"

Car::Car()
{
	model.LoadModel("Assets/car.obj");
	heading = { 0.f, 0.f };
	velocity = { 0.f, 0.f, 0.f };
}

void Car::Update(float delta)
{
	position += velocity * delta;
}