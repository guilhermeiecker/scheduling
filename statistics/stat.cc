#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <stdint.h>
#include <math.h>

#define SIZE 100
#define STEP 100
#define AMAX 10000
#define AMIN 100

using namespace std;

int main(int argc, char** argv)
{
	int nodes = atoi(argv[1]);  // number of nodes
	string input("../results/n" + to_string(nodes) + ".txt"); // set file name at execution time

	// auxiliary arrays
	double m_sum[SIZE], mc_sum[SIZE], dm_min[SIZE], dm_max[SIZE], df_max[SIZE], total_drop[SIZE], samples[SIZE];
	uint64_t f_sum[SIZE];
	double m_avg[SIZE], m_std[SIZE], f_avg[SIZE], f_std[SIZE], z_avg[SIZE], z_std[SIZE], z_sum[SIZE];

	// zeroes the arrays
	fill(m_sum, m_sum + SIZE, 0.0); fill(m_avg, m_avg + SIZE, 0.0); fill(m_std, m_std + SIZE, 0.0);
	fill(mc_sum, mc_sum + SIZE, 0.0); fill(dm_min, dm_min + SIZE, 0.0); fill(dm_max, dm_max + SIZE, 0.0); fill(df_max, df_max + SIZE, 0.0); fill(samples, samples + SIZE, 0.0);
	fill(f_sum, f_sum + SIZE, 0.0); fill(f_avg, f_avg + SIZE, 0.0); fill(f_std, f_std + SIZE, 0.0);
	fill(z_sum, z_sum + SIZE, 0.0); fill(z_avg, z_avg + SIZE, 0.0); fill(z_std, z_std + SIZE, 0.0);

	// input variables
	int n, r, m, mc, dropm_min, dropm_max, dropf_max;
	double a, z;
	uint64_t f;

	ifstream avg(input);
	while (avg >> n >> a >> r >> m >> f >> mc >> z >> dropm_min >> dropm_max >> dropf_max)
	{
		if((dropm_min==0)&&(dropm_max==0)&&(dropf_max==0))
		{
			samples[(int)((a - AMIN)/STEP)]++;
			m_sum[(int)((a - AMIN)/STEP)] += m;
			f_sum[(int)((a - AMIN)/STEP)] += f;
			z_sum[(int)((a - AMIN)/STEP)] += z;
			mc_sum[(int)((a - AMIN)/STEP)] += mc;
		}
		else
			total_drop[(int)((a - AMIN)/STEP)]++;
		dm_min[(int)((a - AMIN)/STEP)] += dropm_min;
		dm_max[(int)((a - AMIN)/STEP)] += dropm_max;
		df_max[(int)((a - AMIN)/STEP)] += dropf_max;
		
	}
	avg.close();
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
			m_avg[i] = m_sum[i] / samples[i];
			f_avg[i] = f_sum[i] / samples[i];
			z_avg[i] = z_sum[i] / samples[i];
		}
	}
	
	fill(m_sum, m_sum + SIZE, 0.0);
	fill(f_sum, f_sum + SIZE, 0.0);
	fill(z_sum, z_sum + SIZE, 0.0);
	ifstream std(input);
	while (std >> n >> a >> r >> m >> f >> mc >> z)
	{
		m_sum[(int)((a - AMIN)/STEP)] += pow((m - m_avg[(int)((a - AMIN)/STEP)]),2);
		f_sum[(int)((a - AMIN)/STEP)] += pow((f - f_avg[(int)((a - AMIN)/STEP)]),2);
		z_sum[(int)((a - AMIN)/STEP)] += pow((z - z_avg[(int)((a - AMIN)/STEP)]),2);
	}
	std.close();
	for(int i = 0; i < SIZE; i++)
	{
		if (samples[i] == 0)
		{
                        m_std[i] = 0;
                        f_std[i] = 0;
                        z_std[i] = 0;
		}
		else
		{
			m_std[i] = sqrt(m_sum[i] / samples[i]);
			f_std[i] = sqrt(f_sum[i] / samples[i]);
			z_std[i] = sqrt(z_sum[i] / samples[i]);
		}
	}

	string output("n" + to_string(nodes) + ".txt"); // set file name at execution time
	ofstream result(output);
	result << "A\tm_avg\tm_std\tf_avg\tf_std\tz_avg\tz_std\tmc\tdropm_min\tdropm_max\tdropf_max\ttotal_drop\tsamples" << endl;
	for(int i  = 0; i < SIZE; i++)
	{
		if (samples[i] == 0)
			result << fixed << setprecision(6) << i*STEP + AMIN << "\t" << m_avg[i] << "\t" << m_std[i] << "\t" << f_avg[i] << "\t" << f_std[i] << "\t" << z_avg[i] << "\t" << z_std[i] << "\t0\t" << dm_min[i]/100 << "\t" << dm_max[i]/100 << "\t" << df_max[i]/100 << "\t" << total_drop[i]/100 << "\t0" << endl;
		else
			result << fixed << setprecision(6) << i*STEP + AMIN << "\t" << m_avg[i] << "\t" << m_std[i] << "\t" << f_avg[i] << "\t" << f_std[i] << "\t" << z_avg[i] << "\t" << z_std[i] << "\t" << mc_sum[i]/samples[i] << "\t" << dm_min[i]/100 << "\t" << dm_max[i]/100 << "\t" << df_max[i]/100 << "\t" << total_drop[i]/100 << "\t" << samples[i] << endl;
	}

	return 0;
}
