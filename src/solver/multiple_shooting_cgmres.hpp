//
// The multiple shooting based continuation GMRES (C/GMRES) method, a fast algorithm of nonlinear model predictive control (NMPC).
// This program is witten with reference to "T. Ohtsuka A continuation/GMRES method for fast computation of nonlinear receding horizon control, Automatica, Vol. 40, No. 4, pp. 563-574 (2004)" and "Y. Shimizu, T. Ohtsuka, M. Diehl, A real‐time algorithm for nonlinear receding horizon control using multiple shooting and continuation/Krylov method, International Journal of Robust and Nonlinear Control, Vol. 19, No. 8, pp. 919-936 (2008)".
//

#ifndef MULTIPLE_SHOOTING_CGMRES_H
#define MULTIPLE_SHOOTING_CGMRES_H


#include "linear_funcs.hpp"
#include "matrixfree_gmres.hpp"
#include "nmpc_model.hpp"
#include "init_cgmres.hpp"


// Solves the nonlinear optimal control problem using the mutiple shooting based C/GMRES method.
class MultipleShootingCGMRES final : virtual public MatrixFreeGMRES{
public:
    // Sets parameters and allocates vectors and matrices.
    MultipleShootingCGMRES(const double T_f, const double alpha, const int horizon_division_num, const double finite_diff_step, const double zeta, const int kmax);

    // Free vectors and matrices.
    ~MultipleShootingCGMRES();

    // Sets parameters for initialization of the solution of NMPC.
    // Args:
    //     initial_guess_solution: A vector whose dimension is dimu+dimc.
    //     residual_tolerance: A criteria for the Newton iteration. It terminates when the error is less than this value.
    //     max_iteration: Maximum number of the Newton iteration.
    void setInitParams(const double* initial_guess_solution, const double residual_tolerance, const int max_iteration, const double finite_diff_step, const int kmax);

    // Initializes the solution of the C/GMRES method.
    void initSolution(const double initial_time, const double* initial_state_vec, double* optimal_control_input_vec);

    // Updates the solution by solving the matrix-free GMRES.
    void controlUpdate(const double current_time, const double sampling_period, const double* current_state_vec, double* optimal_control_input_vec);

    // Returns the optimality error norm under the current_state_vec and the current solution.
    double getError(const double current_time, const double* current_state_vec);


private:
    NMPCModel model_;
    InitCGMRES cgmres_initializer_;
    int dim_state_, dim_control_input_, dim_constraints_, dim_control_input_and_constraints_, dim_state_and_lambda_, dim_control_input_and_constraints_seq_, horizon_division_num_, kmax_;

    // initial_time_, T_f_, alpha_ : parameters of the length of the horizon
    // The horizon length at time t is given by T_f_*(1.0-std::exp(-alpha_*(t - initial_time_))).
    double initial_time_, T_f_, alpha_;
    double zeta_, finite_diff_step_, incremented_time_;
    double *dx_vec_, *incremented_state_vec_, *control_input_and_constraints_seq_, *incremented_control_input_and_constraints_seq_;
    double *control_input_and_constraints_error_seq_, *control_input_and_constraints_error_seq_1_;
    double *control_input_and_constraints_error_seq_2_, *control_input_and_constraints_error_seq_3_,*control_input_and_constraints_update_seq_;
    double **state_mat_, **lambda_mat_, **incremented_state_mat_, **incremented_lambda_mat_, **state_error_mat_, **state_error_mat_1_, **lambda_error_mat_, **lambda_error_mat_1_;

    // Computes the optimaliy error for control input and constraints under current solution.
    inline void computeOptimalityErrorforControlInputAndConstraints(const double time_param, const double* state_vec, const double* control_input_and_constraints_seq, double const* const* state_mat, double const* const* lambda_mat, double* optimality_for_control_input_and_constraints);

    // Computes the optimaliy error for state and lambda under current solution.
    inline void computeOptimalityErrorforStateAndLambda(const double time_param, const double* state_vec, const double* control_input_and_constraints_seq, double const* const* state_mat, double const* const* lambda_mat, double** optimality_for_state, double** optimality_for_lambda);

    // Computes the sequence of state and lambda under the error for state and lambda for the condencing.
    inline void computeStateAndLambda(const double time_param, const double* state_vec, const double* control_input_and_constraints_seq, double const* const* optimality_for_state, double const* const* optimality_for_lambda, double** state_mat, double** lambda_mat);

    // Computes a vector correspongin to b in Ax=b
    void bFunc(const double time_param, const double* state_vec, const double* current_solution_vec, double* b_vec) override;

    // Generates a vector corresponding to Ax in Ax=b with using the forward difference approximation.
    void axFunc(const double time_param, const double* state_vec, const double* current_solution_vec, const double* direction_vec, double* ax_vec) override;
};


#endif