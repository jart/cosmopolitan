// -*- c++ -*-
// clang-format off
#ifndef COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_RMI_H_
#define COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_RMI_H_
#include "third_party/libcxx/iostream"
#include "third_party/libcxx/vector"

using namespace std;

namespace learned_sort {

// Packs the key and its respective scaled CDF value
template <typename T>
struct training_point {
  T x;
  double y;
};

// Represents linear models
struct linear_model {
  double slope = 0;
  double intercept = 0;
};

// An implementation of a 2-layer RMI model
template <class T>
class TwoLayerRMI {
 public:
  // CDF model hyperparameters
  struct Params {
    // Member fields
    long fanout;
    float sampling_rate;
    long threshold;
    long num_leaf_models;

    // Default hyperparameters
    static constexpr long DEFAULT_FANOUT = 1e3;
    static constexpr float DEFAULT_SAMPLING_RATE = .01;
    static constexpr long DEFAULT_THRESHOLD = 100;
    static constexpr long DEFAULT_NUM_LEAF_MODELS = 1000;
    static constexpr long MIN_SORTING_SIZE = 1e4;

    // Default constructor
    Params() {
      this->fanout = DEFAULT_FANOUT;
      this->sampling_rate = DEFAULT_SAMPLING_RATE;
      this->threshold = DEFAULT_THRESHOLD;
      this->num_leaf_models = DEFAULT_NUM_LEAF_MODELS;
    }

    // Constructor with custom hyperparameter values
    Params(float sampling_rate, float overallocation, long fanout,
           long batch_sz, long threshold) {
      this->fanout = fanout;
      this->sampling_rate = sampling_rate;
      this->threshold = threshold;
      this->num_leaf_models = DEFAULT_NUM_LEAF_MODELS;
    }
  };

  // Member variables of the CDF model
  bool trained;
  linear_model root_model;
  vector<linear_model> leaf_models;
  vector<T> training_sample;
  Params hp;
  bool enable_dups_detection;

  // CDF model constructor
  TwoLayerRMI(Params p) {
    this->trained = false;
    this->hp = p;
    this->leaf_models.resize(p.num_leaf_models);
    this->enable_dups_detection = true;
  }

  // Pretty-printing function
  void print() {
    printf("[0][0]: slope=%0.5f; intercept=%0.5f;\n", root_model.slope,
           root_model.intercept);
    for (int model_idx = 0; model_idx < hp.num_leaf_models; ++model_idx) {
      printf("[%i][1]: slope=%0.5f; intercept=%0.5f;\n", model_idx,
             leaf_models[model_idx].slope, leaf_models[model_idx].intercept);
    }
    cout << "-----------------------------" << endl;
  }

  /**
   * @brief Train a CDF function with an RMI architecture, using linear spline
   * interpolation.
   *
   * @param begin Random-access iterators to the initial position of the
   * sequence to be used for sorting. The range used is [begin,end), which
   * contains all the elements between first and last, including the element
   * pointed by first but not the element pointed by last.
   * @param end Random-access iterators to the last position of the sequence to
   * be used for sorting. The range used is [begin,end), which contains all the
   * elements between first and last, including the element pointed by first but
   * not the element pointed by last.
   * @return true if the model was trained successfully, false otherwise.
   */
  bool train(typename vector<T>::iterator begin,
             typename vector<T>::iterator end) {
    // Determine input size
    const long INPUT_SZ = std::distance(begin, end);

    // Validate parameters
    if (this->hp.fanout >= INPUT_SZ) {
      this->hp.fanout = TwoLayerRMI<T>::Params::DEFAULT_FANOUT;
      cerr << "\33[93;1mWARNING\33[0m: Invalid fanout. Using default ("
           << TwoLayerRMI<T>::Params::DEFAULT_FANOUT << ")." << endl;
    }

    if (this->hp.sampling_rate <= 0 or this->hp.sampling_rate > 1) {
      this->hp.sampling_rate = TwoLayerRMI<T>::Params::DEFAULT_SAMPLING_RATE;
      cerr << "\33[93;1mWARNING\33[0m: Invalid sampling rate. Using default ("
           << TwoLayerRMI<T>::Params::DEFAULT_SAMPLING_RATE << ")." << endl;
    }

    if (this->hp.threshold <= 0 or this->hp.threshold >= INPUT_SZ or
        this->hp.threshold >= INPUT_SZ / this->hp.fanout) {
      this->hp.threshold = TwoLayerRMI<T>::Params::DEFAULT_THRESHOLD;
      cerr << "\33[93;1mWARNING\33[0m: Invalid threshold. Using default ("
           << TwoLayerRMI<T>::Params::DEFAULT_THRESHOLD << ")." << endl;
    }

    // Initialize the CDF model
    static const long NUM_LAYERS = 2;
    vector<vector<vector<training_point<T> > > > training_data(NUM_LAYERS);
    for (long layer_idx = 0; layer_idx < NUM_LAYERS; ++layer_idx) {
      training_data[layer_idx].resize(hp.num_leaf_models);
    }

    //----------------------------------------------------------//
    //                           SAMPLE                         //
    //----------------------------------------------------------//

    // Determine sample size
    const long SAMPLE_SZ = std::min<long>(
        INPUT_SZ, std::max<long>(this->hp.sampling_rate * INPUT_SZ,
                                 TwoLayerRMI<T>::Params::MIN_SORTING_SIZE));

    // Create a sample array
    this->training_sample.reserve(SAMPLE_SZ);

    // Start sampling
    long offset = static_cast<long>(1. * INPUT_SZ / SAMPLE_SZ);
    for (auto i = begin; i < end; i += offset) {
      // NOTE:  We don't directly assign SAMPLE_SZ to this->training_sample_sz
      //        to avoid issues with divisibility
      this->training_sample.push_back(*i);
    }

    // Sort the sample using the provided comparison function
    std::sort(this->training_sample.begin(), this->training_sample.end());

    // Count the number of unique keys
    auto sample_cpy = this->training_sample;
    long num_unique_elms = std::distance(
        sample_cpy.begin(), std::unique(sample_cpy.begin(), sample_cpy.end()));

    // Stop early if the array has very few unique values. We need at least 2
    // unique training examples per leaf model.
    if (num_unique_elms < 2 * this->hp.num_leaf_models) {
      return false;
    } else if (num_unique_elms > .9 * training_sample.size()) {
      this->enable_dups_detection = false;
    }

    //----------------------------------------------------------//
    //                     TRAIN THE MODELS                     //
    //----------------------------------------------------------//

    // Populate the training data for the root model
    for (long i = 0; i < SAMPLE_SZ; ++i) {
      training_data[0][0].push_back(
          {this->training_sample[i], 1. * i / SAMPLE_SZ});
    }

    // Train the root model using linear interpolation
    auto *current_training_data = &training_data[0][0];
    linear_model *current_model = &(this->root_model);

    // Find the min and max values in the training set
    training_point<T> min = current_training_data->front();
    training_point<T> max = current_training_data->back();

    // Calculate the slope and intercept terms, assuming min.y = 0 and max.y
    current_model->slope = 1. / (max.x - min.x);
    current_model->intercept = -current_model->slope * min.x;

    // Extrapolate for the number of models in the next layer
    current_model->slope *= this->hp.num_leaf_models - 1;
    current_model->intercept *= this->hp.num_leaf_models - 1;

    // Populate the training data for the next layer
    for (const auto &d : *current_training_data) {
      // Predict the model index in next layer
      long rank = current_model->slope * d.x + current_model->intercept;

      // Normalize the rank between 0 and the number of models in the next layer
      rank = std::max(static_cast<long>(0),
                      std::min(this->hp.num_leaf_models - 1, rank));

      // Place the data in the predicted training bucket
      training_data[1][rank].push_back(d);
    }

    // Train the leaf models
    for (long model_idx = 0; model_idx < this->hp.num_leaf_models;
         ++model_idx) {
      // Update iterator variables
      current_training_data = &training_data[1][model_idx];
      current_model = &(this->leaf_models[model_idx]);

      // Interpolate the min points in the training buckets
      if (model_idx == 0) {
        // The current model is the first model in the current layer

        if (current_training_data->size() < 2) {
          // Case 1: The first model in this layer is empty
          current_model->slope = 0;
          current_model->intercept = 0;

          // Insert a fictive training point to avoid propagating more than one
          // empty initial models.
          training_point<T> tp;
          tp.x = 0;
          tp.y = 0;
          current_training_data->push_back(tp);
        } else {
          // Case 2: The first model in this layer is not empty

          min = current_training_data->front();
          max = current_training_data->back();

          // Hallucinating as if min.y = 0
          current_model->slope = (1. * max.y) / (max.x - min.x);
          current_model->intercept = min.y - current_model->slope * min.x;
        }
      } else if (model_idx == this->hp.num_leaf_models - 1) {
        if (current_training_data->empty()) {
          // Case 3: The final model in this layer is empty

          current_model->slope = 0;
          current_model->intercept = 1;
        } else {
          // Case 4: The last model in this layer is not empty

          min = training_data[1][model_idx - 1].back();
          max = current_training_data->back();

          // Hallucinating as if max.y = 1
          current_model->slope = (1. - min.y) / (max.x - min.x);
          current_model->intercept = min.y - current_model->slope * min.x;
        }
      } else {
        // The current model is not the first model in the current layer

        if (current_training_data->empty()) {
          // Case 5: The intermediate model in this layer is empty
          current_model->slope = 0;
          current_model->intercept = training_data[1][model_idx - 1]
                                         .back()
                                         .y;  // If the previous model
                                              // was empty too, it will
                                              // use the fictive
                                              // training points

          // Insert a fictive training point to avoid propagating more than one
          // empty initial models.
          // NOTE: This will _NOT_ throw to DIV/0 due to identical x's and y's
          // because it is working backwards.
          training_point<T> tp;
          tp.x = training_data[1][model_idx - 1].back().x;
          tp.y = training_data[1][model_idx - 1].back().y;
          current_training_data->push_back(tp);
        } else {
          // Case 6: The intermediate leaf model is not empty

          min = training_data[1][model_idx - 1].back();
          max = current_training_data->back();

          current_model->slope = (max.y - min.y) / (max.x - min.x);
          current_model->intercept = min.y - current_model->slope * min.x;
        }
      }
    }

    // NOTE:
    // The last stage (layer) of this model contains weights that predict the
    // CDF of the keys (i.e. Range is [0-1]) When using this model to predict
    // the position of the keys in the sorted order, you MUST scale the weights
    // of the last layer to whatever range you are predicting for. The inner
    // layers of the model have already been extrapolated to the length of the
    // stage.git
    //
    // This is a design choice to help with the portability of the model.
    //
    this->trained = true;

    return true;
  }
};
}  // namespace learned_sort

#endif /* COSMOPOLITAN_THIRD_PARTY_LEARNED_SORT_RMI_H_ */
