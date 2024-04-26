#include "aircraft.h"

void Aircraft::step(double dt, double t)
{
	logi("Aircraft::step\n");
	Vehicle::step(dt, t);

}


void Aircraft::on_sim_event(SimEvent* event)
{
	logi("Aircraft::on_sim_event\n");
}

