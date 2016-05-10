#ifndef INTEGERPREDICTOR_H
#define INTEGERPREDICTOR_H
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

/*

IntegerPredictor Class Header

Author : Karim
email: akarim7uic@gmail.com

Note that you can turn on multithreading for efficiently utilizing multiple cores via the macro MULTITHREADING in Integerpredictor.cpp
for multithreading boost libraries should be installed
this class takes the predictor model's p parameter as the input, can be trained and tested for any amount of data and training indices
please refer to the comment in above each function definition to see what it does
*/
class IntegerPredictor
{
    int p;
public:
    // p value used in the predictor model, the N value is directly obtained from the csv file

    // constructor which initializes values, loads csv file data
    IntegerPredictor(int p,string training_file_name);
    // function to load data from csv, data can be of any order (0-9 or 9-0, 5-9 and 1-4, etc), this functions also validates for proper data
    // when the digits (left most coloumn) are not in range (0-9) or if the data is not binary, this function returns empty array
    vector<vector< vector<bool> > > load_csv(string training_filename, vector<int> *indices);
    // boolean to store data vadilation result
    bool data_validated;
    // generate combinations, used in generating set of rules
    vector<int> generate_combinations(int n,int p);
    // generate binary combinations for any p (p bit combinations)
    vector<vector<bool> > binary_combinations(int p);
    // struct for efficiently storing rules using (pixels,values) pair
    struct rule
    {
        vector<int> pixels;
        vector<bool> values;
    };
    // vector to store all the data read from the csv
    vector<vector< vector<bool> > > all_data_csv;
    // vetor for storing training data indices (0=9 in this case)
    vector<int> training_index_vec;
    // vector to store all the rules
    vector<struct rule> rules;
    // probability vector (M*10) for M rules
    vector<vector<double> > probability_vector_training;
    // function to get rules
    vector<struct rule> get_rules(int p, int N);
    // returns the number of matches of the rule in a give record
    int check_rule(vector< vector<bool> > record,struct rule rule_to_be_checked);
    // check if a rule exists in a given record
    bool check_rule_per_record(vector<bool> record_row,struct rule rule_to_be_checked);
    // multiply vectors
    vector<double> multiply_vectors(vector<double> vec1,vector<double> vec2);
    // find matches count per record for all the rules and store the count in a vector (matches_count_vector)
    void find_matches_count(vector< vector<bool> > record,vector<struct rule> rules,vector<int> *matches_count_vector);
    // find the probability vector for all the rules from the total matches count (obtained from method above)
    vector<vector <double> > find_probability_vector(vector<vector <int> > total_matches_count,vector<struct rule>*rules);
    // train the model using the methods above
    vector<vector <double> > train();
    // run predictor, this function returns the count of matches predicted for each digit
    // once can use either the training data or another csv file for testing
    vector<int> predict(string test_filename, bool use_training_data,int predict_digit);


};

#endif // INTEGERPREDICTOR_H
