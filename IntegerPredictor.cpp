/*
IntegerPredictor Class Implementation

Author : Karim
email: akarim7uic@gmail.com
*/
#include "IntegerPredictor.h"
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <typeinfo>
#include <iterator>
// set this to 1 for multithreading
#define MULTITHREADING 1
#if MULTITHREADING == 1
#define BOOST_THREAD_USE_LIB
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#endif

// patch for to_string, since g++ throws an error saying its not found
namespace patch_for_to_string
{
template <typename T>
std::string to_string(const T& n)
{
    std::ostringstream stm;
    stm << n;
    return stm.str();
}
}
IntegerPredictor::IntegerPredictor(int p_value, string training_filename)
{
    p = p_value;
    // load data
    cout << "#############################################" << endl;
    cout << " Loading Training Data from the csv file " << training_filename << endl;
    cout << "#############################################" << endl;
    all_data_csv = IntegerPredictor::load_csv(training_filename, &training_index_vec);
    cout << "all data loaded with " << all_data_csv.size() << " digits, with each having "
         << all_data_csv[0].size() << " data points of " << all_data_csv[0][1].size() << " bits"
         << endl;

    // size of each data
    int N = all_data_csv[0][1].size();
    // load rules
    rules = get_rules(p, N);
}

vector<vector<vector<bool> > > IntegerPredictor::load_csv(string filename, vector<int>* indices)
{
    // if load_digit=999, this function loads the entire data excluding the left
    // most coloumn of the csv
    std::ifstream data(filename.c_str());
    vector<vector<vector<bool> > > all_pixels;
    vector<vector<bool> > pixels_per_digit;
    vector<int> index_vec;

    std::string line;
    vector<vector<bool> > data_list;
    bool breakFlag = false;
    bool saveFlag = false;
    int coloumn;
    int current_digit = 0;
    int previous_digit = 0;
    bool first_cell = true;
    vector<bool> data_record;
    while (std::getline(data, line))
    {
        std::istringstream lineStream(line);
        std::string cell;
        coloumn = 0;

        // check if cell is valid, should contain only 0's and 1's except the
        // leftmost coloumn
        while (std::getline(lineStream, cell, ','))
        {
            coloumn = coloumn + 1;
            // in case of loading data of specific digit
            if (coloumn == 1)
            {
                if (first_cell)
                {
                    // prev digit must be initialized to first cell
                    first_cell = false;
                    previous_digit = atoi(cell.c_str());
                    //  cout<<"problem"<<endl;
                    // saveFlag=false;
                }
                else
                    previous_digit = current_digit;
                current_digit = atoi(cell.c_str());
                if (previous_digit != current_digit)
                {
                    index_vec.push_back(previous_digit);
                    saveFlag = true;
                }

                // cout<<"current digit" << current_digit<<endl;
                if (!((current_digit >= 0) && (current_digit <= 9)))
                {
                    // digit is not valid (0-9)
                    breakFlag = true;
                    break;
                }

                // cout<<"coloumn 1 cell is " << cell<< " and patch is "
                // <<patch_for_to_string::to_string(load_digit)  << " and compare result
                // is " <<
                // cell.compare(patch_for_to_string::to_string(load_digit))<<endl;
                // for  saving data correspinding to a digit
                // ((!(cell.compare(patch_for_to_string::to_string(load_digit))==0)))
                // cout<<"ignored" << cell <<endl;
            }
            // cout<<"cell is " << cell <<endl;;
            else // ignore first coloumn
            {
                if (!((cell.compare("0") == 0)
                        || (cell.compare("1") == 0))) // the cell values should be either 1 or 0
                {
                    cout << "not valid" << endl;
                    breakFlag = true;
                    break;
                }
                else
                {
                    // cout<<"saving"<<endl;
                    data_record.push_back(abs(cell.compare("0")));
                }
            }
        }
        if (saveFlag)
        {
            cout << "saving training data of digit " << previous_digit << endl;
            all_pixels.push_back(pixels_per_digit);

            pixels_per_digit.clear();
            saveFlag = false;
        }
        pixels_per_digit.push_back(data_record);
        data_record.clear();
        if (breakFlag)
        {
            cout << "csv load failed, please check the csv file" << endl;
            all_pixels.clear();
            cout << "nope" << endl;
            return all_pixels;
        }

        // cout<<"all pixels size " << all_pixels.size()<<endl;
    }
    index_vec.push_back(previous_digit);
    cout << "saving training data of digit " << previous_digit << endl;
    all_pixels.push_back(pixels_per_digit); // if line = "" then the last digit
    // will not get saved, so saving here

    *indices = index_vec;
    return all_pixels;
}

vector<int> IntegerPredictor::generate_combinations(int n, int r)
{
    vector<bool> v(n);
    vector<int> combinations;
    fill(v.begin(), v.end() - n + r, true);

    do
    {
        for (int i = 0; i < n; ++i)
        {
            if (v[i])
            {
                combinations.push_back(i + 1);
            }
        }
    } while (std::prev_permutation(v.begin(), v.end()));

    return combinations;
}

vector<vector<bool> > IntegerPredictor::binary_combinations(int p)
{
    vector<vector<bool> > binary_combinations;
    vector<bool> binary_combination;

    for (int i = 0; i < std::pow(2, p); i++)
    {
        for (int j = 0; j < p; j++)
        {
            // cout <<"2 power j is " << (i/(int)pow(2,j))%2<<endl;
            binary_combination.push_back((i / (int)pow(2, j)) % 2);
        }

        binary_combinations.push_back(binary_combination);
        binary_combination.clear();
    }
    return binary_combinations;
}

vector<IntegerPredictor::rule> IntegerPredictor::get_rules(int p, int N)
{
    IntegerPredictor::rule rule;
    vector<IntegerPredictor::rule> rules;
    vector<int> combinations = generate_combinations(N, p);
    cout << "total combinations " << combinations.size() << endl;
    // for p=1
    vector<vector<bool> > bool_combinations = binary_combinations(p);
    vector<int> single_combination();
    vector<int> pixels;
    cout << "boolean vector size (" << p << "bit) " << bool_combinations.size() << endl;
    for (int i = 0; i < combinations.size(); i = i + p)
    {
        for (int j = 0; j < bool_combinations.size(); j++)
        {
            rule.pixels = vector<int>(combinations.begin() + i, combinations.begin() + i + p);
            rule.values = bool_combinations[j];
            rules.push_back(rule);
        }
    }

    return rules;
}

int IntegerPredictor::check_rule(
    vector<vector<bool> > record, IntegerPredictor::rule rule_to_be_checked)
{
    int matches_count = 0;
    bool count_flag=true;
    // cout<<"checking rule with record size "<< record.size()<<endl;
    // for each row in record
    for (int i = 0; i < record.size(); i++)
    {
        // cout<<"record index " << i <<endl;
        count_flag=true;
        vector<bool> record_row = record[i];
        // cout<<"record row size " <<record[i].size()<<endl;
        // for each pixel,value pair
        for (int j = 0; j < rule_to_be_checked.pixels.size(); j++)
        {
            // cout<<"rule number " << j <<endl;
            if (!(record_row[rule_to_be_checked.pixels[j]] == rule_to_be_checked.values[j]))
                {
                    count_flag=false;
                    break;
                }
        }

        if count_flag:
            matches_count = matches_count + 1;

    }
    return matches_count;
}

bool IntegerPredictor::check_rule_per_record(
    vector<bool> record_row, struct rule rule_to_be_checked)
{
    for (int j = 0; j < rule_to_be_checked.pixels.size(); j++)
    {
        // cout<<"rule number " << j <<endl;
        if (record_row[rule_to_be_checked.pixels[j]] == rule_to_be_checked.values[j])
            return true;
    }
    return false;
}

void IntegerPredictor::find_matches_count(vector<vector<bool> > record,
    vector<IntegerPredictor::rule> rules, vector<int>* matches_count_vector)
{
    vector<int> temp_initialize_vector;

    for (int j = 0; j < rules.size(); j++)
    {
        temp_initialize_vector.push_back(check_rule(record, rules[j]));
    }

    *matches_count_vector = temp_initialize_vector;
    temp_initialize_vector.clear();
}

vector<vector<double> > IntegerPredictor::find_probability_vector(
    vector<vector<int> > total_matches_count, vector<IntegerPredictor::rule>* rules)
{
    vector<vector<double> > probability_vector;
    vector<double> probability_vector_rule;
    vector<int> total_matches_all_data;

    int total;
    for (int j = 0; j < total_matches_count[0].size(); j++)
    {
        total = 0;
        for (int i = 0; i < total_matches_count.size(); i++)
        {
            total = total + total_matches_count[i][j];
        }
        // erase if number of matches <30
        if (total < 30)
        {
            rules->erase(rules->begin() + j);
            cout << "rule erased" << endl;
        }
        total_matches_all_data.push_back(total);
    }

    for (int j = 0; j < total_matches_count[0].size(); j++)
    {
        for (int i = 0; i < total_matches_count.size(); i++)
        {
            probability_vector_rule.push_back(
                (double)total_matches_count[i][j] / (double)total_matches_all_data[j]);
        }
        if (total_matches_all_data[j] > 30)
        {
            probability_vector.push_back(probability_vector_rule);
            probability_vector_rule.clear();
        }

        // cout<<"created probabilities" <<endl;
    }

    return probability_vector;
}

vector<double> IntegerPredictor::multiply_vectors(vector<double> vec1, vector<double> vec2)
{
    vector<double> mul_vec;

    for (int i = 0; i < vec1.size(); i++)
    {
        mul_vec.push_back(vec1[i] * vec2[i]);
    }
    return mul_vec;
}

vector<vector<double> > IntegerPredictor::train()
{
    cout << "#############################################" << endl;
    cout << " Training started for p = " << p << "......." << endl;
    cout << "#############################################" << endl;

    // generate total count and probability vector
    vector<vector<int> > probability_model_rules;
    // initialize total matches count vector
    vector<vector<int> > total_matches_count(training_index_vec.size(), vector<int>(rules.size()));
// multithreading: spin up a seperate thread for each digit
// std::vector<boost::thread *> digit_thread;
#if MULTITHREADING == 1
    cout << "running training in a multithreaded environment with 10 threads "
            "(one per digit)"
         << endl;
    std::vector<boost::thread*> z;
    for (int i = 0; i < training_index_vec.size(); i++)
    {
        z.push_back(new boost::thread(&IntegerPredictor::find_matches_count, this, all_data_csv[i],
            rules, &total_matches_count[i]));
    }
    for (int q = 0; q < z.size(); q++)
    {
        z[q]->join();
        delete z[q];
    }
#else
    for (int i = 0; i < 10; i++)
    {
        cout << "finding matches for digit " << training_index_vec[i] << endl;
        find_matches_count(all_data_csv[i], rules, &total_matches_count[i]);
    }
#endif // MULTITHREADING
    // caculate probability vectors for each rule
    int total_matches = 0;
    int rules_ignored_count = 0;

    vector<vector<double> > probability_vector
        = find_probability_vector(total_matches_count, &rules);
    cout << "prob_vector size " << probability_vector.size() << endl;
    // print probability vector by uncommenting these lines
    //    for (int i=0;i<probability_vector.size();i++)
    //    {
    //        for (int j=0;j<probability_vector[i].size();j++)
    //        {
    //            cout<< probability_vector[i][j];
    //        }
    //        cout<<endl;
    //    }
    // predict for digit 0 with p=1
    // for each data record corresponding to zero digit
    if (probability_vector.size() == 0)
        cout << "Training failed :(";
    probability_vector_training = probability_vector;
    return probability_vector;
}
vector<int> IntegerPredictor::predict(
    string test_filename, bool use_training_data, int predict_digit)
{
    vector<int> counts_digits;
    if (probability_vector_training.size() == 0)
    {
        cout << "Training Process is not completed succesfully, check the data" << endl;
        return counts_digits;
    }

    cout << "#############################################" << endl;
    cout << " Predicting Digit = " << predict_digit << "......." << endl;
    cout << "#############################################" << endl;
    vector<vector<vector<bool> > > test_data;
    vector<int> test_indices;
    // use a different data for testing (different from training data)
    if (!use_training_data)
    {
        test_data = load_csv(test_filename, &test_indices);
        cout << "using test data provided........" << endl;
    }
    // use training data for testing
    else
    {
        test_data = all_data_csv;
        test_indices = training_index_vec;
        cout << "using training data for testing ............" << endl;
    }

    vector<bool> data_row;
    vector<double> final_probability_vector;

    for (int i = 0; i < training_index_vec.size(); i++)
    {
        final_probability_vector.push_back(1);
        counts_digits.push_back(0);
    }

    if ((predict_digit < 0) && (predict_digit > training_index_vec.size() - 1))
    {
        cout << "predict digit not valid, this is the data of the digit used for "
                "testing"
             << endl;
        return counts_digits;
    }
    for (int i = 0; i < test_data[predict_digit].size(); i++)
    {
        // for each rule
        for (int j = 0; j < rules.size(); j++)
        {
            if (check_rule_per_record(test_data[predict_digit][i], rules[j]))
                // cout<<"rule valid"<<endl;
                final_probability_vector
                    = multiply_vectors(final_probability_vector, probability_vector_training[j]);
        }
        // cout<<"the max element is " << double
        // (*std::max_element(final_probability_vector.begin(),
        // final_probability_vector.end()))<<endl;
        // cout<<"the value at 1 is " << final_probability_vector[1] <<endl;

        int biggest = std::distance(final_probability_vector.begin(),
            std::max_element(final_probability_vector.begin(), final_probability_vector.end()));
        std::fill(final_probability_vector.begin(), final_probability_vector.end(), 1);
        // cout<<"biggest " << biggest<<endl;
        counts_digits[biggest] = counts_digits[biggest] + 1;
    }
    cout << "#############################################" << endl;
    cout << " Results predicted for the of digit = " << predict_digit << endl;
    cout << "#############################################" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << "counts for " << i << " " << counts_digits[i] << endl;
    }

    cout << "The accuracy of prediction for digit " << predict_digit << " is "
         << (float)counts_digits[predict_digit] / (float)test_data[predict_digit].size() << endl;
    return counts_digits;
}
