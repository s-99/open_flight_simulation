#include "aircraft.h"

void Aircraft::step(double dt, double t)
{
	printf("Aircraft::step\n");

	Vehicle::step(dt, t);

}


void Aircraft::on_sim_event(SimEvent* event)
{
	printf("Aircraft::on_sim_event\n");
}

