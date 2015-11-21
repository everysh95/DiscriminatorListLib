#include"Discriminator.h"

namespace base
{
	discriminator::discriminator()
	{}

	discriminator::discriminator(std::vector<double> w)
	:w(w)
	{}

	discriminator::discriminator(size_t size)
	:w(size,0)
	{}

	double& discriminator::W(size_t index)
	{
		return w[index];
	}

	size_t discriminator::W_size()
	{
		return w.size();
	}

	bool discriminator::match(std::vector<double> target)
	{

		double ans = 0;
		for(int i = 0;i < w.size();i++)
		{
			ans += w[i]*target[i];
		}

		if(ans >= 0)
			return true;
		else
			return false;

	}

}
