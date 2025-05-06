#include "block.h"
#include "block_intergrator.h"
#include "block_lag_filter.h"
#include "block_pid.h"
#include "block_simple.h"
#include "block_trans_func.h"


Block* create_block(const string& type)
{
	if (type == "transfer_function")
	{
		return new BlockTransFunc();
	}
	else if (type == "derivative")
	{
		return new BlockDerivative();
	}
	else if (type == "saturation")
	{
		return new BlockSaturation();
	}
	else if (type == "rate_limiter")
	{
		return new BlockRateLimiter();
	}
	else if (type == "sum")
	{
		return new BlockSum();
	}
	else if (type == "gain")
	{
		return new BlockGain();
	}
	else if (type == "pid")
	{
		return new BlockPid();
	}
	else if (type == "lag_filter")
	{
		return new BlockLagFilter();
	}
	else if (type == "integrator")
	{
		return new BlockIntegrator();
	}
	return nullptr;
}
