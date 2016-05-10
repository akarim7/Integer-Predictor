#include <iostream>
#include <vector>
#include <bitset>
#include <numeric>
#include "IntegerPredictor.h"
#define MULTITHREADING_TESTING 1
#if MULTITHREADING_TESTING==1
#define BOOST_THREAD_USE_LIB
#include <boost/regex.hpp>
#include <boost/thread.hpp>
#endif
// use following command to run in linux terminal
// g++ main.cpp IntegerPredictor.cpp -o main.o  -lboost_system -lboost_thread &&./main.o

using namespace std;

// funtion to find predicted matches for a digit, this format is used for multithreading
void find_prediction_for_all_digits(IntegerPredictor i_predictor,int digit,vector<int> *predicted_matches_vec)
{
    vector<int> predicted_matches_v;
    predicted_matches_v=i_predictor.predict("", true, digit);

    *predicted_matches_vec=predicted_matches_v;
}

int main()
{
    // enter the p value and training data location in the constructor
    IntegerPredictor i_predictor(1, "OCRpixels.csv");
    // train
    i_predictor.train();
    // for prediction
    // give the test data as first argument and if you want to use training data for testing, set
    // the second argument to be true
    // the third argument is the data used for testing, for e.g 2 denotes the data corresponding to
    // 2 will be used for testing


    // vector to store all the predicted counts for all the digits
    vector<vector<int> > predicted_matches_vector(i_predictor.training_index_vec.size(), vector<int>(i_predictor.training_index_vec.size()));

    #if MULTITHREADING_TESTING==1
    cout<<"Testing for all digits using multiple threads in parallel (10 threads)"<<endl;
    std::vector<boost::thread*> z;
    for (int digit = 0; digit < i_predictor.training_index_vec.size(); digit++)
    {
        z.push_back(new boost::thread(find_prediction_for_all_digits, i_predictor,
            digit, &predicted_matches_vector[digit]));
    }
    for (int q = 0; q < z.size(); q++)
    {
        z[q]->join();
        delete z[q];
    }
    #else
    for (int digit = 0; digit < i_predictor.training_index_vec.size(); digit++)
    {
        find_prediction_for_all_digits( i_predictor,
            digit, &predicted_matches_vector[digit]);
    }
    #endif // MULTITHREADING

    // find accuracy for all digits
    vector<float> prediction_accuracy;
    float accuracy;
    float total_accuracy = 0;

    for (int digit = 0; digit < i_predictor.training_index_vec.size(); digit++)
    {
        accuracy = (float)predicted_matches_vector[digit][digit]
            / (float)i_predictor.all_data_csv[digit].size();
        cout << "prediction accuracy for the digit " << digit << " is " << accuracy << endl;
        total_accuracy = total_accuracy + accuracy;
        prediction_accuracy.push_back(accuracy);
    }



    // overall accuracy
    cout << "overall accuracy is " << total_accuracy / (float)prediction_accuracy.size() << endl;
}
