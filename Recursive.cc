#include "Recursive.h"

void Recursive::find_fset(uint64_t x)
{
	it = x;
	uint64_t limit;
	if (x == 0)
	{
		limit = m;
		for (uint64_t i = 0; i < limit; i++)
			find_fset(x + (uint64_t)pow(2, i));
	}
	else
	{
		limit = (uint64_t)log2(x & ~(x - 1));
		add_link(limit);
		if (is_feasible()) {
			fset.push_back(x);
			for (uint64_t i = 0; i < limit; i++)
				find_fset(x + (uint64_t)pow(2, i));
		}
		del_link(limit);
	}
}

void Recursive::add_link(uint64_t index)
{
	if (!cset.empty()) {
		double interfAB, interfBA;
		for (vector<Link*>::iterator i = cset.begin(); i != cset.end(); ++i)
		{
			interfAB = calculate_interference((*i)->get_sender(), network->get_link(index)->get_recver());
			interfBA = calculate_interference(network->get_link(index)->get_sender(), (*i)->get_recver());

			network->get_link(index)->add_interf(interfAB);
			(*i)->add_interf(interfBA);
		}
	}
	network->get_link(index)->get_sender()->inc_degree();
	network->get_link(index)->get_recver()->inc_degree();
	cset.push_back(network->get_link(index));
}

double Recursive::calculate_interference(Node* a, Node* b)
{
  double dist = a->distance(*b);
  if (dist > network->d0)
  	return pow(10.0, ((network->tpower_dBm - network->l0_dB - 10 * network->alpha*log10(dist / network->d0)) / 10.0));
  else
    return pow(10.0, network->tpower_dBm - network->l0_dB / 10.0);
}

bool Recursive::is_feasible()
{
	if(cset.size() < 2)
		return true;
	if(cset.size() > n/2)
		return false;
	if(primary_test()&&secondary_test())
		return true;
	return false;
}

bool Recursive::primary_test()
{
	for (vector<Link*>::iterator i = cset.begin(); i != cset.end(); ++i)
	{
		if(((*i)->get_sender()->get_degree() > 1)||((*i)->get_recver()->get_degree() > 1))
    	return false;
	}
	return true;
}

bool Recursive::secondary_test()
{
	double sinr;
	for(vector<Link*>::iterator i = cset.begin(); i != cset.end(); ++i)
	{
		sinr = calculate_sinr(*i);
		if(sinr < network->beta_mW)
			return false;
	}
	return true;
}

double Recursive::calculate_sinr(Link* l)
{
    return l->get_rpower() / (network->noise_mW + l->clc_interf());
}

void Recursive::del_link(uint64_t index)
{
	cset.pop_back();
	network->get_link(index)->get_sender()->dec_degree();
	network->get_link(index)->get_recver()->dec_degree();
	network->get_link(index)->clr_interf();
	for (vector<Link*>::iterator i = cset.begin(); i != cset.end(); ++i)
		    (*i)->del_interf();
}

Recursive::Recursive(Network* g): n(g->get_nodes().size()), m(g->get_links().size()), network(g) {}

vector<uint64_t> Recursive::get_fset()
{
	return fset;
}

void Recursive::print_fset()
{
	cout << "Printing fset for Algorithm 4..." << endl;
	for (vector<uint64_t>::iterator i = fset.begin(); i != fset.end(); ++i)
		cout << *i << " ";
	cout << endl;
}

void Recursive::clear_fset()
{
	fset.clear();
}
