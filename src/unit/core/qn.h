#pragma once
// Supporting functions for testing the quasinormal step code 

#include "optizelle/optizelle.h"
#include "optizelle/vspaces.h"
#include "unit.h"

// Grab the squaring function
using Optizelle::sq;

// JustA lump everything in the unit structure 
template <typename Real>
struct Unit {
    // Set some type shortcuts
    template <typename Real_>
    using XX = Optizelle::Rm <Real_>;
    typedef XX <Real> X;
    typedef typename X::Vector X_Vector;

    template <typename Real_>
    using YY = Optizelle::Rm <Real_>;
    typedef YY <Real> Y;
    typedef typename Y::Vector Y_Vector;

    // Various objective functions
    struct Objective {
        // Zero objective function
        struct Zero : public Optizelle::ScalarValuedFunction <Real,XX> {
            Real eval(X_Vector const & x) const {
                return Real(0.);
            }
            void grad(
                X_Vector const & x,
                X_Vector & grad
            ) const {
                X::zero(grad);
            }
            void hessvec(
                X_Vector const & x,
                X_Vector const & dx,
                X_Vector & H_dx
            ) const {
                X::zero(H_dx);
            }
        };
    };

    // Various constraints
    struct Constraint {
        // Define a linear constraint
        //
        // g(x,y)= [ x + y = 1 ] 
        //
        struct Linear : public Optizelle::VectorValuedFunction<Real,XX,YY> {
            // y=g(x) 
            void eval(
                X_Vector const & x,
                Y_Vector & y
            ) const {
                y[0] = x[0] + x[1] - Real(1.); 
            }

            // y=g'(x)dx
            void p(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector & y
            ) const {
                y[0] = dx[0] + dx[1];
            }

            // z=g'(x)*dy
            void ps(
                X_Vector const & x,
                Y_Vector const & dy,
                X_Vector & x_hat
            ) const {
                x_hat[0]=dy[0];
                x_hat[1]=dy[0];
            }

            // z=(g''(x)dx)*dy
            void pps(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector const & dy,
                X_Vector & x_hat 
            ) const {
                X::zero(x_hat);
            }
        };
        
        // Define a quadratic constraint
        //
        // g(x,y)= [ a x^2 + b y^2 = 1 ] 
        //
        struct Quadratic : public Optizelle::VectorValuedFunction<Real,XX,YY> {
            // Determines the width of the ellipse
            Real a;

            // Determines the height of the ellpise
            Real b;

            // Grabs the size of the quadratic
            Quadratic(Real const & a_, Real const & b_) : a(a_), b(b_) {}

            // y=g(x) 
            void eval(
                X_Vector const & x,
                Y_Vector & y
            ) const {
                y[0] = a * sq(x[0]) + b * sq(x[1]) - Real(1.); 
            }

            // y=g'(x)dx
            void p(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector & y
            ) const {
                y[0] = Real(2.)*a*x[0]*dx[0] + Real(2.)*b*x[1]*dx[1];
            }

            // z=g'(x)*dy
            void ps(
                X_Vector const & x,
                Y_Vector const & dy,
                X_Vector & x_hat
            ) const {
                x_hat[0]=Real(2.)*a*x[0]*dy[0];
                x_hat[1]=Real(2.)*b*x[1]*dy[0];
            }

            // z=(g''(x)dx)*dy
            void pps(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector const & dy,
                X_Vector & x_hat 
            ) const {
                x_hat[0]=Real(2.)*a*dx[0]*dy[0];
                x_hat[1]=Real(2.)*b*dx[1]*dy[0];
            }
        };
        
        // Define a constraint where we intersect two circles 
        //
        // g(x,y)= [ (x-a)^2 + (y-b)^2 = 1 ] 
        //         [ (x-c)^2 + (y-d)^2 = 1 ] 
        //
        struct CircleIntersection :
            public Optizelle::VectorValuedFunction<Real,XX,YY>
        {
            // Location of the first circle 
            Real a;
            Real b;

            // Location of the second circle 
            Real c;
            Real d;

            // Grabs the size of the quadratic
            CircleIntersection(
                Real const & a_, 
                Real const & b_,
                Real const & c_,
                Real const & d_
            ) : a(a_), b(b_), c(c_), d(d_) {}

            // y=g(x) 
            void eval(
                X_Vector const & x,
                Y_Vector & y
            ) const {
                y[0] = sq(x[0]-a) + sq(x[1]-b) - Real(1.); 
                y[1] = sq(x[0]-c) + sq(x[1]-d) - Real(1.); 
            }

            // y=g'(x)dx
            void p(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector & y
            ) const {
                y[0] = Real(2.)*(x[0]-a)*dx[0] + Real(2.)*(x[1]-b)*dx[1];
                y[1] = Real(2.)*(x[0]-c)*dx[0] + Real(2.)*(x[1]-d)*dx[1];
            }

            // z=g'(x)*dy
            void ps(
                X_Vector const & x,
                Y_Vector const & dy,
                X_Vector & x_hat
            ) const {
                x_hat[0] = Real(2.)*(x[0]-a)*dy[0] + Real(2.)*(x[1]-c)*dy[1];
                x_hat[1] = Real(2.)*(x[0]-b)*dy[0] + Real(2.)*(x[1]-d)*dy[1];
            }

            // z=(g''(x)dx)*dy
            void pps(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector const & dy,
                X_Vector & x_hat 
            ) const {
                x_hat[0] = Real(2.)*dx[0]*dy[0] + Real(2.)*dx[1]*dy[1];
                x_hat[1] = Real(2.)*dx[0]*dy[0] + Real(2.)*dx[1]*dy[1];
            }
        };
        
        // Define a trigonometric constraint 
        //
        // g(x) = cos(x)
        //
        struct Cos : public Optizelle::VectorValuedFunction<Real,XX,YY> {
            // y=g(x) 
            void eval(
                X_Vector const & x,
                Y_Vector & y
            ) const {
                y[0] = std::cos(x[0]); 
            }

            // y=g'(x)dx
            void p(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector & y
            ) const {
                y[0] = -std::sin(x[0])*dx[0];
            }

            // z=g'(x)*dy
            void ps(
                X_Vector const & x,
                Y_Vector const & dy,
                X_Vector & x_hat
            ) const {
                x_hat[0] = -std::sin(x[0])*dy[0];
            }

            // z=(g''(x)dx)*dy
            void pps(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector const & dy,
                X_Vector & x_hat 
            ) const {
                x_hat[0] = -std::cos(x[0])*dx[0]*dy[0];
            }
        };
        
        // Define a polynomial constraint 
        //
        // g(x) = (x-1)*(x-2)*(x-3)*(x-4) 
        //
        struct Poly : public Optizelle::VectorValuedFunction<Real,XX,YY> {
            // y=g(x) 
            void eval(
                X_Vector const & x,
                Y_Vector & y
            ) const {
                y[0] = (x[0]-Real(1.))*(x[0]-Real(2.))*
                       (x[0]-Real(3.))*(x[0]-Real(4.));
            }

            // y=g'(x)dx
            void p(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector & y
            ) const {
                y[0] = (Real(2.)*(Real(2.)*x[0]-Real(5.))*
                           (sq(x[0])-Real(5.)*x[0]+Real(5.)))*dx[0]; 
            }

            // z=g'(x)*dy
            void ps(
                X_Vector const & x,
                Y_Vector const & dy,
                X_Vector & x_hat
            ) const {
                x_hat[0] = (Real(2.)*(Real(2.)*x[0]-Real(5.))*
                               (sq(x[0])-Real(5.)*x[0]+Real(5.)))*dy[0]; 
            }

            // z=(g''(x)dx)*dy
            void pps(
                X_Vector const & x,
                X_Vector const & dx,
                Y_Vector const & dy,
                X_Vector & x_hat 
            ) const {
                x_hat[0] = Real(2.)*(Real(6.)*sq(x[0])-Real(30.)*x[0]+Real(35.))
                    *dy[0]*dx[0]; 
            }
        };
    };

    // Simple quasinormal subproblem setup 
    struct QN {
        // Tolerance for all our tests
        Real eps;

        // Points where we run the test 
        X_Vector x;
        Y_Vector y;

        // Equality constraint 
        std::unique_ptr <Optizelle::VectorValuedFunction <Real,XX,YY>> g;

        // Inequality constraint
        std::unique_ptr <Optizelle::VectorValuedFunction <Real,XX,YY>> h;

        // Trust-region radius
        Real delta;

        // Desired solution
        std::unique_ptr <X_Vector> dx_ncp_star;
        std::unique_ptr <X_Vector> dx_n_star;

        // Desired stopping condition 
        Optizelle::QuasinormalStop::t qn_stop_star;

        // Check the stopping condition
        bool check_stop;

        // Check that the trust-region is active 
        bool check_tr;

        // Check the quasinormal step
        bool check_dx_n;

        // Check the quasinormal cauchy point
        bool check_dx_ncp;

        // Check the feasibility
        bool check_feas;

        // Check that we cut back the step from the safeguard
        bool check_alpha_x_qn; 

        // Setup some simple parameters
        QN(X_Vector const & x_,Y_Vector const & y_) :
            eps(std::pow(
                Real(10.),
                std::log10(std::numeric_limits <Real>::epsilon())
                    *Real(0.75))),
            x(X::init(x_)),
            y(Y::init(y_)),
            g(),
            h(),
            delta(1e16),
            dx_ncp_star(),
            dx_n_star(),
            qn_stop_star(),
            check_stop(false),
            check_tr(false),
            check_dx_n(false),
            check_dx_ncp(false),
            check_feas(false),
            check_alpha_x_qn(false)
        {
            X::copy(x_,x); 
            Y::copy(y_,y);
        }
    };

    // Calculates a error between two vectors.  The second number returned 
    // is the norm of the second vector, which we use to calculate the relative
    // error.
    static std::tuple <Real,Real> error(
        typename XX <Real>::Vector const & x,
        typename XX <Real>::Vector const & y
    ) {
        typedef XX <Real> X;
        auto r = X::init(x);
        X::copy(x,r);
        X::axpy(Real(-1.),y,r);
        auto norm_r = std::sqrt(X::innr(r,r));
        auto norm_y = std::sqrt(X::innr(y,y));
        return std::tuple <Real,Real>(norm_r,norm_y);
    }

    // Run and verify the problem setup
    static void run_and_verify(QN & setup) {
        // Create an optimization state
        typename Optizelle::EqualityConstrained <Real,XX,YY>::State::t
            state(setup.x,setup.y);

        // Set some appropraite state information
        state.delta = setup.delta;

        // Create a bundle of functions
        typename Optizelle::EqualityConstrained <Real,XX,YY>::Functions::t fns;
        fns.f.reset(new typename Objective::Zero);
        fns.g = std::move(setup.g);

        // Create the messaging object
        auto msg = Optizelle::Messaging();

        // Fill out the bundle of functions
        Optizelle::EqualityConstrained <Real,XX,YY>::Functions::init(
            msg,state,fns);

        // Evaluate the function and cache information about it
        fns.g->eval(state.x,state.g_x);
        state.norm_gxtyp = std::sqrt(Y::innr(state.g_x,state.g_x));
        auto gps_g = X::init(state.x);
        fns.g->ps(state.x,state.g_x,gps_g);
        state.norm_gpsgxtyp = std::sqrt(X::innr(gps_g,gps_g));

        // Compute the quasinormal step
        Optizelle::EqualityConstrained<Real,XX,YY>::Algorithms::quasinormalStep(
            fns,state);

        // Check the stopping condition 
        if(setup.check_stop)
            CHECK(state.qn_stop == setup.qn_stop_star);
        
        // Check that we hit the trust-region modified by zeta 
        if(setup.check_tr) {
            auto x_p_dxn = X::init(state.x);
            X::copy(state.x,x_p_dxn);
            X::axpy(Real(1.),state.dx_n,x_p_dxn);
            auto norm_xpdxn = std::sqrt(X::innr(x_p_dxn,x_p_dxn));
            CHECK(std::abs(norm_xpdxn-state.delta*state.zeta) <= setup.eps);
        }

        // Check that the quasinormal point is correct
        if(setup.check_dx_n) {
            auto norm_r = Real(0.);
            auto norm_dxn_star = Real(0.);
            std::tie(norm_r,norm_dxn_star)=error(state.dx_n,*setup.dx_n_star);
            CHECK(norm_r <= setup.eps);
        }

        // Check that the cauchy point is correct
        if(setup.check_dx_ncp) {
            auto norm_r = Real(0.);
            auto norm_dxncp_star = Real(0.);
            std::tie(norm_r,norm_dxncp_star)=error(
                state.dx_ncp,*setup.dx_ncp_star);
            CHECK(norm_r <= setup.eps);
        }

        // Check that our step takes us to feasibility 
        if(setup.check_feas) {
            auto x_p_dxn = X::init(state.x);
            X::copy(state.x,x_p_dxn);
            X::axpy(Real(1.),state.dx_n,x_p_dxn);
            auto g_x = Y::init(state.y);
            fns.g->eval(x_p_dxn,g_x);
            auto norm_gx = std::sqrt(X::innr(g_x,g_x));
            CHECK(norm_gx <= setup.eps);
        }

        // Check that the safeguard truncated the step 
        if(setup.check_alpha_x_qn) {
            CHECK(state.alpha_x_qn < Real(1.));
        }

        // Move the function out the bundle of functions and back into setup in
        // case we need it later
        setup.g = std::move(fns.g);
    }
};
