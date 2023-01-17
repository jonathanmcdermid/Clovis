#include "tuner.h"

using namespace std;

namespace Clovis {

	namespace Tuner {

		vector<Position> positions;
		vector<double> results;
		long double best_mse;
		long double k;

		constexpr int n_cores = 6;
		long double answers = 0;
	
		void tune_eval()
		{
			// load positions and results from file
			string file_name = "src/tuner.epd";
			ifstream ifs;
			ifs.open(file_name.c_str(), ifstream::in);
			string line;

			while (true)
			{
				if (ifs.eof())
					break;
				getline(ifs, line);
				if (line.length())
				{
					size_t idx = line.find("\"");
					size_t idx_end = line.find("\"", idx + 1);
					string res = line.substr(idx + 1, idx_end - idx - 1);
					if (res == "1-0")
						results.push_back(1.0);
					else if (res == "0-1")
						results.push_back(0.0);
					else if (res == "1/2-1/2")
						results.push_back(0.5);
					else return;
					positions.push_back(Position(line.substr(0, idx).c_str()));
				}
			}

			ifs.close();

			cout << "positions done loading " << positions.size() << endl;

			vector<short*> weights = map_weights_to_params();

			bool improved;

			// compute scaling constant k
			k = 1.68;//find_k();

			do {
				improved = false;

				best_mse = mean_squared_error(k);

				long double mse = tune_loop(weights);

				for (short step = 1; step < 5; ++step)
				{
					cout << "step: " << step << endl;
					for (size_t index = 0; index < weights.size() - 1; index += 2)
					{
						short best_val_mg = *weights[index];
						short best_val_eg = *weights[index + 1];

						*weights[index] += step;

						for (int i = 0; i < 5 && mse >= best_mse && *weights[index + 1] > 0; ++i)
						{
							*weights[index + 1] -= 1;
							*weights[index + 1] = max(short(0), *weights[index + 1]);

							mse = mean_squared_error(k);

							// if a thread fails we catch it here
							while (best_mse - mse > best_mse / (n_cores + 1))
								mse = mean_squared_error(k);
						}

						if (mse < best_mse)
						{
							best_val_mg = *weights[index];
							best_val_eg = *weights[index + 1];
							best_mse = mse;
							print_params();
							cout << mse << endl;
							improved = true;
						}
						else
						{
							*weights[index] = best_val_mg;
							*weights[index + 1] = best_val_eg;
						}

						if (*weights[index] == 0)
							continue;

						*weights[index] -= step;
						*weights[index] = max(short(0), *weights[index]);

						for (int i = 0; i < 10 && mse >= best_mse; ++i)
						{
							*weights[index + 1] += 1;
							mse = mean_squared_error(k);

							// if a thread fails we catch it here
							while (best_mse - mse > best_mse / (n_cores + 1))
								mse = mean_squared_error(k);
						}

						if (mse < best_mse)
						{
							best_mse = mse;
							print_params();
							cout << mse << endl;
							improved = true;
						}
						else
						{
							*weights[index] = best_val_mg;
							*weights[index + 1] = best_val_eg;
						}
					}
				}
			} while (improved);

			print_params();

		}

		long double tune_loop(vector<short*> weights)
		{
			long double mse;

			vector<int> direction(weights.size(), 1);

			bool improved = true;

			for (unsigned iterations = 0; improved; ++iterations)
			{
				improved = false;
				for (size_t index = 0; index < weights.size(); ++index)
				{
					short old_val = *weights[index];

					// increase weight
					*weights[index] += direction[index];

					*weights[index] = max(short(0), *weights[index]);

					mse = mean_squared_error(k);

					// if a thread fails we catch it here
					while (best_mse - mse > best_mse / (n_cores + 1))
						mse = mean_squared_error(k);

					if (mse < best_mse)
					{
						best_mse = mse;
						improved = true;
					}
					else
					{
						*weights[index] = old_val;

						// decrease weight
						*weights[index] -= direction[index];

						*weights[index] = max(short(0), *weights[index]);

						mse = mean_squared_error(k);

						// if a thread fails we catch it here
						while (best_mse - mse > best_mse / (n_cores + 1))
							mse = mean_squared_error(k);

						if (mse < best_mse)
						{
							direction[index] = -direction[index];
							best_mse = mse;
							improved = true;
						}
						else
							// reset weight, no improvements
							*weights[index] = old_val;
					}
					cout << "weight " << index << " mse: " << best_mse << endl;
				}
				
				print_params();
				cout << "iteration " << iterations << " complete" << endl;
			}
			cout << "\ndone!\n";

			return best_mse;
		}

		long double mean_squared_error(long double K)
		{
			// compute mean squared error for all positions in the set
			// multithreading solution

			answers = 0;

			int batch_size = positions.size() / n_cores;

			vector<thread> thread_pool;
			int start = 0;
			int end = 0;

			for (int i = 0; i < n_cores; ++i)
			{
				start = end;
				end = start + batch_size;
				thread_pool.push_back(thread(processor, start, end, K));
			}

			for (int i = 0; i < n_cores; ++i)
				thread_pool[i].join();

			return answers / positions.size();
		}

		void processor(int start, int end, long double K)
		{
			// thread process for calculating the mse of the divided set

			long double error_sum = 0;
			long double sigmoid = 0;
			int eval;

			for (int i = start; i < end; ++i)
			{
				eval = Eval::evaluate<false>(positions[i]);
				// all evaluations need to be from white perspective
				if (positions[i].side == BLACK)
					eval = -eval;
				sigmoid = 1 / (1 + pow(10, -K * eval / 400));
				error_sum += pow(results[i] - sigmoid, 2);
			}

			answers += error_sum;
		}

		long double find_k()
		{
			int k_precision = 10;
			long double start = -10;
			long double end = 10;
			long double step = 1;
			long double curr = start;
			long double error;
			long double best = mean_squared_error(start);

			for (int i = 0; i < k_precision; ++i)
			{
				curr = start - step;

				while (curr < end)
				{
					curr += step;
					error = mean_squared_error(curr);
					if (error < best)
					{
						best = error;
						start = curr;
					}
				}

				cout.precision(17);
				cout << "epoch " << i << " K = " << start << " E = " << best << endl;

				end = start + step;
				start -= step;
				step /= 10.0;
			}

			return start;
		}

		vector<short*> map_weights_to_params()
		{
			// point weights to the variables in the evaluation function
			
			vector<short*> weights;
			
			return weights;
		}

		void print_params()
		{
			// print the tuned weights so they can be copy pasted into evaluation file

		}

	} // namespace Tuner

} // namespace Clovis
