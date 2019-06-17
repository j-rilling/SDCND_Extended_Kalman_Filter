#include "FusionEKF.h"
#include <iostream>
#include "Eigen/Dense"
#include "tools.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::cout;
using std::endl;
using std::vector;
using std::pow;

/**
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // Creates local variables to initialize the Kalman filter matrices
  MatrixXd P_initial(4,4);
  MatrixXd F_initial(4,4);
  MatrixXd Q_initial(4,4);

  MatrixXd R_laser_initial(2,2);
  MatrixXd R_radar_initial(3,3);

  MatrixXd H_laser_initial(2,4);
  MatrixXd H_radar_initial(3,4);

  // P is initialized with 
  P_initial << 1, 0, 0, 0,
               0, 1, 0, 0,
               0, 0, 1000, 0,
               0, 0, 0, 1000;

  F_initial << 1, 0, 0, 0,
               0, 1, 0, 1,
               0, 0, 1, 0, 
               0, 0, 0, 1;

  Q_initial << 0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0,
               0, 0, 0, 0;

  R_laser_initial << 0.0225, 0,
                     0, 0.0225;

  R_radar_initial << 0.09, 0, 0,
                     0, 0.0009, 0,
                     0, 0, 0.09;

  H_laser_initial << 1, 0, 0, 0,
                     0, 1, 0, 0;

  H_radar_initial << 0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0, 0, 0;
  

  // Initial values of other matrices of Kalman Filter
  ekf_ = KalmanFilter(4, P_initial, F_initial, Q_initial, H_laser_initial, 
                      R_laser_initial, H_radar_initial, R_radar_initial);
}

/**
 * Destructor.
 */
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {
  /**
   * Initialization
   */
  if (!is_initialized_) {
    /**
     * Initialize the state ekf_.x_ with the first measurement.
     * Create the covariance matrix.
     * You'll need to convert radar from polar to cartesian coordinates.
     */

    // first measurement
    cout << "EKF: " << endl;
    VectorXd firstMeasurement(4);
    float px_0, py_0;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      // TODO: Convert radar from polar to cartesian coordinates 
      //         and initialize state.
      px_0 = measurement_pack.raw_measurements_[0]*sin(measurement_pack.raw_measurements_[1]);
      py_0 = measurement_pack.raw_measurements_[0]*cos(measurement_pack.raw_measurements_[1]);
    }
    else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      // TODO: Initialize state.
      px_0 = measurement_pack.raw_measurements_[0];
      py_0 = measurement_pack.raw_measurements_[1];
    }

    firstMeasurement << px_0, py_0, 0, 0;
    ekf_.InitX(firstMeasurement);

    // done initializing, no need to predict or update
    is_initialized_ = true;

    
    /** Print 
    cout << "Kalman Filter initialized with: " << endl;
    ekf_.printAllVariables();
    **/

    return;
  }

  /**
   * Prediction
   */

  /**
   * Update the state transition matrix F according to the new elapsed time.
   * Time is measured in seconds.
   */

  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;
  previous_timestamp_ = measurement_pack.timestamp_;

  MatrixXd updated_F(4,4);
  updated_F << 1, 0, dt, 0,
               0, 1, 0, dt,
               0, 0, 1, 0,
               0, 0, 0, 1;

  ekf_.updateF(updated_F);
  
  /**
   * Update the process noise covariance matrix.
   * Use noise_ax = 9 and noise_ay = 9 for your Q matrix.
   */

  
  float noise_ax = 9;
  float noise_ay = 9;

  MatrixXd updated_Q(4,4);
  updated_Q << (pow(dt,4)/4)*noise_ax, 0, (pow(dt,3)/2)*noise_ax, 0,
               0, (pow(dt,4)/4)*noise_ay, 0, (pow(dt,3)/2)*noise_ay, 
               (pow(dt,3)/2)*noise_ax, 0, pow(dt,2)*noise_ax, 0,
               0, (pow(dt,3)/2)*noise_ay, 0, pow(dt,2)*noise_ay;

  ekf_.updateQ(updated_Q);
  ekf_.printAllVariables();

  // x and P before the update (debugging print)
  //cout << "x before the update: " << ekf_.getX() << endl;
  //cout << "P before the update: " << ekf_.getP() << endl;

  ekf_.Predict();
  //cout << "x after the update: " << ekf_.getX() << endl;
  //cout << "P after the update: " << ekf_.getP() << endl;

  /**
   * Update
   */

  /**
   * TODO:
   * - Use the sensor type to perform the update step.
   * - Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
    // TODO: Radar updates

  } else {
    // TODO: Laser updates

  }

  // print the output
  //cout << "x_ = " << ekf_.getX() << endl;
  //cout << "P_ = " << ekf_.getP() << endl;
}
