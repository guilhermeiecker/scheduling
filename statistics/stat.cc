#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdint.h>
#include <math.h>

#define SIZE 87
using namespace std;

int main(int argc, char** argv)
{
	int n = atoi(argv[1]);  // number of nodes
	string input("../results/n" + to_string(n) + ".txt"); // set file name at execution time

	// auxiliary arrays
	double m_sum[SIZE], mc_sum[SIZE], samples[SIZE];
	uint64_t f_sum[SIZE];
	double m_avg[SIZE], m_std[SIZE], f_avg[SIZE], f_std[SIZE], z_avg[SIZE], z_std[SIZE], z_sum[SIZE];

	// zeroes the arrays
	fill(m_sum, m_sum + SIZE, 0); fill(m_avg, m_avg + SIZE, 0.0); fill(m_std, m_std + SIZE, 0.0);
	fill(mc_sum, mc_sum + SIZE, 0);	fill(samples, samples + SIZE, 0);
	fill(f_sum, f_sum + SIZE, 0); fill(f_avg, f_avg + SIZE, 0.0); fill(f_std, f_std + SIZE, 0.0);
	fill(z_sum, z_sum + SIZE, 0.0); fill(z_avg, z_avg + SIZE, 0.0); fill(z_std, z_std + SIZE, 0.0);

	// input variables
	int r, m, mc;
	double a, z;
	uint64_t f;

	ifstream avg(input);
	while (avg >> n >> a >> r >> m >> f >> mc >> z)
	{
		samples[(int)((a - 400)/100)]++;
		m_sum[(int)((a - 400)/100)] += m;
		f_sum[(int)((a - 400)/100)] += f;
		z_sum[(int)((a - 400)/100)] += z;
		mc_sum[(int)((a - 400)/100)] += mc;
	}
	avg.close();
	for(int i = 0; i < SIZE; i++)
	{
		//cout << "m_sum=" << m_sum[i] << ", f_sum=" << f_sum[i] << ", z_sum=" << z_sum[i] << ", samples=" << samples[i] << endl;
		if (samples[i] == 0)
		{
			m_avg[i] = 0;
			f_avg[i] = 0;
			z_avg[i] = 0;
		}
		else
		{
			m_avg[i] = m_sum[i] / samples[i];
			f_avg[i] = f_sum[i] / samples[i];
			z_avg[i] = z_sum[i] / samples[i];
		}
	}
	
	fill(m_sum, m_sum + SIZE, 0);
	fill(f_sum, f_sum + SIZE, 0);
	fill(z_sum, z_sum + SIZE, 0.0);
	ifstream std(input);
	while (std >> n >> a >> r >> m >> f >> mc >> z)
	{
		m_sum[(int)((a - 400)/100)] += pow((m - m_avg[(int)((a - 400)/100)]),2);
		f_sum[(int)((a - 400)/100)] += pow((f - f_avg[(int)((a - 400)/100)]),2);
		z_sum[(int)((a - 400)/100)] += pow((z - z_avg[(int)((a - 400)/100)]),2);
	}
	std.close();
	for(int i = 0; i < SIZE; i++)
	{
		if (samples[i] == 0)
		{
                        m_avg[i] = 0;
                        f_avg[i] = 0;
                        z_avg[i] = 0;
		}
		else
		{
			m_std[i] = sqrt(m_sum[i] / samples[i]);
			f_std[i] = sqrt(f_sum[i] / samples[i]);
			z_std[i] = sqrt(z_sum[i] / samples[i]);
		}
	}

	string output("n" + to_string(n) + ".txt"); // set file name at execution time
	ofstream result(output);
	result << "A\tm_avg\tm_std\tf_avg\tf_std\tz_avg\tz_std\tmc" << endl;
	for(int i  = 0; i < SIZE; i++)
	{
		if (samples[i] == 0)
			result << fixed << setprecision(6) << i*100 + 400 << "\t" << m_avg[i] << "\t" << m_std[i] << "\t" << f_avg[i] << "\t" << f_std[i] << "\t" << z_avg[i] << "\t" << z_std[i] << "\t0\t0" << endl;
		else
			result << fixed << setprecision(6) << i*100 + 400 << "\t" << m_avg[i] << "\t" << m_std[i] << "\t" << f_avg[i] << "\t" << f_std[i] << "\t" << z_avg[i] << "\t" << z_std[i] << "\t" << mc_sum[i]/samples[i] << "\t" << samples[i] << endl;
	}

	return 0;
}
