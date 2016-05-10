#include <iostream>
#include <vector>
#include <bitset>
#include <numeric>
#include "IntegerPredictor.h"
// use following command to run in linux terminal
//g++ main.cpp IntegerPredictor.cpp -o main.o  -lboost_system -lboost_thread &&./main.o

using namespace std;

int main()
{
    // enter the p value and training data location in the constructor
    IntegerPredictor i_predictor(2,"OCRpixels.csv");
    // train
    i_predictor.train();
    // for prediction
    // give the test data as first argument and if you want to use training data for testing, set the second argument to be true
    // the third argument is the data used for testing, for e.g 2 denotes the data corresponding to 2 will be used for testing
    vector<vector <int> > predicted_matches_vector;
    // predict matches for all digits
    for (int digit=0;digit<i_predictor.training_index_vec.size();digit++)
        predicted_matches_vector.push_back(i_predictor.predict("",true,digit));

    // find accuracy for all digits
    vector<float> prediction_accuracy;
    float accuracy;
    float total_accuracy=0;

    for (int digit=0;digit<i_predictor.training_index_vec.size();digit++)
    {
        accuracy=(float) predicted_matches_vector[digit][digit]/(float)i_predictor.all_data_csv[digit].size();
        cout<<"prediction accuracy for the digit " << digit << " is " << accuracy <<endl;
        total_accuracy=total_accuracy+accuracy;
        prediction_accuracy.push_back(accuracy);
    }

    // overall accuracy
    cout <<"overall accuracy is " << total_accuracy/(float)prediction_accuracy.size()<<endl;




}
