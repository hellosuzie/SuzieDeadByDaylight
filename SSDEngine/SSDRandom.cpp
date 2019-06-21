
#include "SSDRandom.h"


std::random_device SSDRandom::RD;
std::mt19937 SSDRandom::Mt = std::mt19937(RD());

SSDRandom::SSDRandom()
{
}


SSDRandom::~SSDRandom()
{
}
