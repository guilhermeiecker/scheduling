#include "Network.h"

vector<Node> Network::get_nodes()
{
	return nodes;
}

vector<Link> Network::get_links()
{
	return links;
}

void Network::set_nodes()
{
	Node* n;
	for (uint64_t i = 0; i < num_nodes; i++)
	{
		n = new Node(i, area_side);
		nodes.push_back(*n);
	}
}

void Network::set_links()
{
	uint64_t index = 0;
	for (vector<Node>::iterator i = nodes.begin(); i != nodes.end(); ++i) {
		for (vector<Node>::iterator j = i + 1; j != nodes.end(); ++j) {
			int coin = ((int) random())%2;
			double pr;
			double dist = i->distance(*j);
			if (dist <= max_range) {
				if (dist > d0)
					pr = pow(10.0, ((tpower_dBm - l0_dB - 10*alpha*log10(dist / d0))/10.0));
				else
					pr = pow(10.0, ((tpower_dBm - l0_dB) / 10.0));
				if (coin == 0)
					links.push_back(Link(&(*i), &(*j), index++, dist, pr));
				else
					links.push_back(Link(&(*j), &(*i), index++, dist, pr));
			}
		}
	}
}

Link* Network::get_link(uint64_t idx)
{
	return &(links[idx]);
}

void Network::print_links()
{
	cout << "Printing links..." << endl;
	for (vector<Link>::iterator i = links.begin(); i != links.end(); ++i)
		cout << "Link id=" << i->get_id() << " sender(id=" << (*i).get_sender()->get_id() << ", deg=" << (*i).get_sender()->get_degree() << ") receiver(id=" << (*i).get_recver()->get_id() << ", deg=" << (*i).get_recver()->get_degree() << ")" << endl;
}
