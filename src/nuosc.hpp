#if not defined(__NUOSC__)
#define __NUOSC__
/*---------------------------------------------------------------------------*/

typedef double real;

class NuOsc
{
public:
    real phy_time, dt;

    // all field variables...
    real *Z, *vw, *vz;
    real *Hm;
    FieldVar *v_stat, *v_rhs, *v_pre, *v_cor;
    Flux *flux;
    Profile *G0;

    int nz;  // Dim of z  (the last dimension, the one with derivatives. Cell-center grid used.)
    int nvz; // Dim of vz (Vertex-center grid used.)
    int gz;  // Width of z-buffer zone.
    int size;
    real ko;

    real dz, dv, dv_new;
    real vz0, vz1;
    real z0, z1;
    real CFL;

    real signu;
    real siganu;
    real alpha;
    real eln_0;
    real beln_0;

    real theta;
    real ct;
    real st;
    real mu;
    real pmo; // 1.0 for normal mass ordering, -1.0 for inverted mass ordering, 0.0 for no vacuum term

    real perturbation_size;

    std::string SCHEME;
    std::ofstream outf;

    NuOsc(struct Params pars) : phy_time(0.)
    {
        SCHEME = pars.SCHEME;
        vz0 = pars.v0;
        vz1 = pars.v1;
        z0 = pars.z0;
        z1 = pars.z1;

        nz = pars.nz;
        nvz = pars.nvz;
        CFL = pars.CFL;
        gz = pars.gz; // Width of z-buffer zone. 4 for 2nd-order of d/dz.

        ko = 1.0e-1; // artificial KO dissipation for FD scheme 

        perturbation_size = pars.perturbation_size;

        size = (nz + 2 * gz) * (nvz);
        Z = new double[nz];
        vz = new double[nvz];
        vw = new real[nvz]; // quadrature of v

        v_stat = new FieldVar(size);
        v_rhs = new FieldVar(size);
        v_pre = new FieldVar(size);
        v_cor = new FieldVar(size);
        flux = new Flux(size);
        G0 = new Profile(size);

        Hm = new double[nz];

        dz = (z1 - z0) / nz;
        dv = (vz1 - vz0) / (nvz); // cell-center
        dv_new = (vz1 - vz0) / (nvz-1);
        dt = CFL * dz; // / vz1;


        for (int i = 0; i < nz; i++)
        {
            Z[i] = z0 + (i + 0.5) * dz;
        }



/*---------------------------------------------------------------------------*/
//              a lot of choice for flight direction (vz)

        /*
        //COSEnu
        for (int i = 0; i < nvz; i++)
        {
            vz[i] = vz0 + (i + 0.5) * dv;
            vw[i] = 1.0;
        }
        */

        
        //For specific velocity: vz = -1 & vz = 1
        vz[0] = -1.0;
        vz[1] = 1.0;
        // set to vw = 1.0 means "equal weighting" for each velocity
        vw[0] = 1.0;
        vw[1] = 1.0;
        
        
       
       
        
    
        /*
        //For FFS multiple beams, like COSEnu but contain -1 & 1
        vz[0] = -1.0;
        vz[nvz-1] = 1.0;
        vw[0] = 1.0;
        vw[nvz-1] = 1.0;

        for (int i = 1; i < nvz-1; i++)
        {
            vz[i] = vz0 + i * dv_new;
            vw[i] = 1.0;
        }
        */
        
        /*
        //For FFS multiple beams, vz: 0~1
        vz[0] = 0.0;
        vz[nvz-1] = 1.0;
        vw[0] = 1.0;
        vw[nvz-1] = 1.0;

        for (int i = 1; i < nvz-1; i++)
        {
            vz[i] = vz0 + i * dv_new;
            vw[i] = 1.0;
        }
        */



/*---------------------------------------------------------------------------*/


        for (int i = 0; i < nz; i++)
        {
            Hm[i] = -1.0e-3 * (Z[i] + 600.0);
        }

        std::cout << draw(50, "#") << std::endl;
        std::cout << std::setw(35)
                  << "Simulation state created" << std::endl
                  << draw(50, ".") << std::endl
                  << "\t"
                  << "SCHEME: " << SCHEME << std::endl
                  << "\t"
                  << "z : (" << z0 << ", " << z1 << ")" << std::endl
                  << "\t"
                  << "vz: (" << vz[0] << ", " << vz[nvz - 1] << ")" << std::endl
                  << "\t"
                  << "(Nvz, Nz): (" << nvz << ", " << nz << ")" << std::endl
                  << "\t"
                  << "gz: " << gz << std::endl
                  << "\t"
                  << "CFL: " << CFL
                  << "\n\t"
                  << "perturbation_size: " << perturbation_size
                  << std::endl;
        std::cout << draw(50, "#") << std::endl;
    }
    ~NuOsc()
    {
        delete[] vz;
        delete[] Z;
        delete v_stat;
        delete v_rhs;
        delete v_pre;
        delete v_cor;
        delete flux;
        delete G0;
        delete[] Hm;
    }

    void set_vac_pars(double pmo_, double omega_, double theta_)
    {
        pmo = pmo_; // 1.0 for normal mass ordering, -1.0 for inverted mass ordering, 0.0 for no vacuum term
        theta = theta_ * M_PI / 180.0;
        ct = (1.0 / 2.0) * omega_ * cos(2.0 * theta);
        st = (1.0 / 2.0) * omega_ * sin(2.0 * theta);
    }

    void set_collective_pars(double mu_)
    {
        mu = mu_;
    }

    inline unsigned int idx(const int i, const int j)
    {
        return i * (nz + 2 * gz) + (j + gz);
    }

    inline int v_idx(double v)
    {
        return (int)((v - vz0) / dv - 0.5);
    }

    inline int z_idx(double z)
    {
        return (int)((z - z0) / dz - 0.5);
    }

    // Simulation specific methods.
    // Edit as per requirement
    void initialize();

    // Editing these methods are not recomended
    void updateBufferZone(FieldVar *in);
    void step_rk4();
    void WENO7(const double *, double *, double *);
    void calFlux(const FieldVar *, Flux *);
    void calRHS(FieldVar *out, const FieldVar *in);
    void vectorize(FieldVar *v0, const FieldVar *v1, const real a, const FieldVar *v2);
    void vectorize(FieldVar *v0, const FieldVar *v1, const real a, const FieldVar *v2, const FieldVar *v3);
    void copy_state(const FieldVar *, FieldVar *);

    // State outputs
    void dump_rho(const FieldVar *, const uint);

    // Binary file ops
    void write_state(unsigned int);
    void write_state0(const FieldVar *);
    int read_state();
    void read_state0(FieldVar *);
    void read_G0();

    // Analysis related methods.
    // Edit these functions as per the requirement.
    void cal_pol(const FieldVar *, Pol *);
    void cal_Mn(M *, const Pol *, unsigned int);
    void dcon(const Pol *, const Pol *, M *, int);
    void analyse(const FieldVar *, const Pol *, uint, uint);
    void surv_prob(const FieldVar *, const FieldVar *, const uint);
};

void NuOsc::copy_state(const FieldVar *ivstate, FieldVar *cpvstate)
{
    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < nz; j++)
        {
            cpvstate->ee[idx(i, j)] = ivstate->ee[idx(i, j)];
            cpvstate->xx[idx(i, j)] = ivstate->xx[idx(i, j)];
            cpvstate->ex_re[idx(i, j)] = ivstate->ex_re[idx(i, j)];
            cpvstate->ex_im[idx(i, j)] = ivstate->ex_im[idx(i, j)];

            cpvstate->bee[idx(i, j)] = ivstate->bee[idx(i, j)];
            cpvstate->bxx[idx(i, j)] = ivstate->bxx[idx(i, j)];
            cpvstate->bex_re[idx(i, j)] = ivstate->bex_re[idx(i, j)];
            cpvstate->bex_im[idx(i, j)] = ivstate->bex_im[idx(i, j)];
        }
    }
}

//注意，只有當我們在configs.yaml裏面設置時，這裡的B.C.才會被define!!!
void NuOsc::updateBufferZone(FieldVar *in)
{

#ifdef PERIODIC_BC
#pragma omp parallel for
#pragma acc parallel loop collapse(2) independent //  default(present)
    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < gz; j++)
        {
            // lower side
            in->ee[idx(i, -j - 1)] = in->ee[idx(i, nz - j - 1)];
            in->xx[idx(i, -j - 1)] = in->xx[idx(i, nz - j - 1)];
            in->ex_re[idx(i, -j - 1)] = in->ex_re[idx(i, nz - j - 1)];
            in->ex_im[idx(i, -j - 1)] = in->ex_im[idx(i, nz - j - 1)];

            in->bee[idx(i, -j - 1)] = in->bee[idx(i, nz - j - 1)];
            in->bxx[idx(i, -j - 1)] = in->bxx[idx(i, nz - j - 1)];
            in->bex_re[idx(i, -j - 1)] = in->bex_re[idx(i, nz - j - 1)];
            in->bex_im[idx(i, -j - 1)] = in->bex_im[idx(i, nz - j - 1)];

            // upper side
            in->ee[idx(i, nz + j)] = in->ee[idx(i, j)];
            in->xx[idx(i, nz + j)] = in->xx[idx(i, j)];
            in->ex_re[idx(i, nz + j)] = in->ex_re[idx(i, j)];
            in->ex_im[idx(i, nz + j)] = in->ex_im[idx(i, j)];

            in->bee[idx(i, nz + j)] = in->bee[idx(i, j)];
            in->bxx[idx(i, nz + j)] = in->bxx[idx(i, j)];
            in->bex_re[idx(i, nz + j)] = in->bex_re[idx(i, j)];
            in->bex_im[idx(i, nz + j)] = in->bex_im[idx(i, j)];
        }
    }
#endif


#ifdef OPEN_BC
#pragma omp parallel for
#pragma acc parallel loop collapse(2) indedependent // default(present)
    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < gz; j++)
        {
            // lower side
            in->ee[idx(i, -j - 1)] = in->ee[idx(i, 0)];
            in->xx[idx(i, -j - 1)] = in->xx[idx(i, 0)];
            in->ex_re[idx(i, -j - 1)] = in->ex_re[idx(i, 0)];
            in->ex_im[idx(i, -j - 1)] = in->ex_im[idx(i, 0)];

            in->bee[idx(i, -j - 1)] = in->bee[idx(i, 0)];
            in->bxx[idx(i, -j - 1)] = in->bxx[idx(i, 0)];
            in->bex_re[idx(i, -j - 1)] = in->bex_re[idx(i, 0)];
            in->bex_im[idx(i, -j - 1)] = in->bex_im[idx(i, 0)];

            // upper side, estimated simply by extrapolation
            in->ee[idx(i, nz + j)] = in->ee[idx(i, nz + j - 1)] * 2 - in->ee[idx(i, nz + j - 2)];
            in->xx[idx(i, nz + j)] = in->xx[idx(i, nz + j - 1)] * 2 - in->xx[idx(i, nz + j - 2)];
            in->ex_re[idx(i, nz + j)] = in->ex_re[idx(i, nz + j - 1)] * 2 - in->ex_re[idx(i, nz + j - 2)];
            in->ex_im[idx(i, nz + j)] = in->ex_im[idx(i, nz + j - 1)] * 2 - in->ex_im[idx(i, nz + j - 2)];

            in->bee[idx(i, nz + j)] = in->bee[idx(i, nz + j - 1)] * 2 - in->bee[idx(i, nz + j - 2)];
            in->bxx[idx(i, nz + j)] = in->bxx[idx(i, nz + j - 1)] * 2 - in->bxx[idx(i, nz + j - 2)];
            in->bex_re[idx(i, nz + j)] = in->bex_re[idx(i, nz + j - 1)] * 2 - in->bex_re[idx(i, nz + j - 2)];
            in->bex_im[idx(i, nz + j)] = in->bex_im[idx(i, nz + j - 1)] * 2 - in->bex_im[idx(i, nz + j - 2)];
        }
    }
#endif

//B.C. for fast flavor swap
#ifdef FFS_BC
#pragma omp parallel for
#pragma acc parallel loop collapse(2) independent
    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < gz; j++)
        {
            if (vz[i] > 0) // for right-going beams
            {
                // left boundary (Dirichlet)
                // neutrino
                in->ee[idx(i, -j - 1)] = 1.0; // for pure electron neutrino
                in->xx[idx(i, -j - 1)] = 0.0;
                in->ex_re[idx(i, -j - 1)] = 0.0;
                in->ex_im[idx(i, -j - 1)] = 0.0;
                // anti-neutrino
                in->bee[idx(i, -j - 1)] = 0.0; 
                in->bxx[idx(i, -j - 1)] = 0.0;
                in->bex_re[idx(i, -j - 1)] = 0.0;
                in->bex_im[idx(i, -j - 1)] = 0.0;
                

                // right boundary (open, 3rd order extrapolation)
                // neutrino
                in->ee[idx(i, nz + j)] = 3 * in->ee[idx(i, nz + j - 1)] - 3 * in->ee[idx(i, nz + j - 2)] + in->ee[idx(i, nz + j - 3)];
                in->xx[idx(i, nz + j)] = 3 * in->xx[idx(i, nz + j - 1)] - 3 * in->xx[idx(i, nz + j - 2)] + in->xx[idx(i, nz + j - 3)];
                in->ex_re[idx(i, nz + j)] = 3 * in->ex_re[idx(i, nz + j - 1)] - 3 * in->ex_re[idx(i, nz + j - 2)] + in->ex_re[idx(i, nz + j - 3)];
                in->ex_im[idx(i, nz + j)] = 3 * in->ex_im[idx(i, nz + j - 1)] - 3 * in->ex_im[idx(i, nz + j - 2)] + in->ex_im[idx(i, nz + j - 3)];
                // anti-neutrino
                in->bee[idx(i, nz + j)] = 3 * in->bee[idx(i, nz + j - 1)] - 3 * in->bee[idx(i, nz + j - 2)] + in->bee[idx(i, nz + j - 3)];
                in->bxx[idx(i, nz + j)] = 3 * in->bxx[idx(i, nz + j - 1)] - 3 * in->bxx[idx(i, nz + j - 2)] + in->bxx[idx(i, nz + j - 3)];
                in->bex_re[idx(i, nz + j)] = 3 * in->bex_re[idx(i, nz + j - 1)] - 3 * in->bex_re[idx(i, nz + j - 2)] + in->bex_re[idx(i, nz + j - 3)];
                in->bex_im[idx(i, nz + j)] = 3 * in->bex_im[idx(i, nz + j - 1)] - 3 * in->bex_im[idx(i, nz + j - 2)] + in->bex_im[idx(i, nz + j - 3)];


            }
            else // for left-going beams
            {
                // left boundary (open, 3rd order extrapolation)
                // neutrino
                in->ee[idx(i, -j - 1)] = 3 * in->ee[idx(i, -j)] - 3 * in->ee[idx(i, -j + 1)] + in->ee[idx(i, -j + 2)];
                in->xx[idx(i, -j - 1)] = 3 * in->xx[idx(i, -j)] - 3 * in->xx[idx(i, -j + 1)] + in->xx[idx(i, -j + 2)];
                in->ex_re[idx(i, -j - 1)] = 3 * in->ex_re[idx(i, -j)] - 3 * in->ex_re[idx(i, -j + 1)] + in->ex_re[idx(i, -j + 2)];
                in->ex_im[idx(i, -j - 1)] = 3 * in->ex_im[idx(i, -j)] - 3 * in->ex_im[idx(i, -j + 1)] + in->ex_im[idx(i, -j + 2)];
                // anti-neutrino
                in->bee[idx(i, -j - 1)] = 3 * in->bee[idx(i, -j)] - 3 * in->bee[idx(i, -j + 1)] + in->bee[idx(i, -j + 2)];
                in->bxx[idx(i, -j - 1)] = 3 * in->bxx[idx(i, -j)] - 3 * in->bxx[idx(i, -j + 1)] + in->bxx[idx(i, -j + 2)];
                in->bex_re[idx(i, -j - 1)] = 3 * in->bex_re[idx(i, -j)] - 3 * in->bex_re[idx(i, -j + 1)] + in->bex_re[idx(i, -j + 2)];
                in->bex_im[idx(i, -j - 1)] = 3 * in->bex_im[idx(i, -j)] - 3 * in->bex_im[idx(i, -j + 1)] + in->bex_im[idx(i, -j + 2)];

                
                // right boundary (Dirichlet)
                in->ee[idx(i, nz + j)] = 0.0; 
                in->xx[idx(i, nz + j)] = 0.0;
                in->ex_re[idx(i, nz + j)] = 0.0;
                in->ex_im[idx(i, nz + j)] = 0.0;
                // anti-neutrino
                in->bee[idx(i, nz + j)] = 2.0; // now set \alpha = n_bee/n_ee = 1.0 
                in->bxx[idx(i, nz + j)] = 0.0;
                in->bex_re[idx(i, nz + j)] = 0.0;
                in->bex_im[idx(i, nz + j)] = 0.0;
            }

        }
    }
#endif


//B.C. for time-depent Dirichlet
#ifdef TimeDep_BC
#pragma omp parallel for
#pragma acc parallel loop collapse(2) independent
    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < gz; j++)
        {
            if (vz[i] > 0) // only consider right-going beams
            {
                // left boundary (Dirichlet, with t-dependent \alpha!)
                //Purpose: see whether FFS happens when the ELN at the boundary transits from nue dominated to nuebar dominated

                //setup for alpha 
                
                
                double t_transition = 0.0; // 開始增加的時間點
                double transition_width = 200.0; // 過渡寬度
                double initial_bee = 0.5;
                double final_bee = 1.2;
                double bee_diff = final_bee - initial_bee;
                double sigmoid = 1.0 / (1.0 + exp(-(phy_time - t_transition) / transition_width));
                double alpha_t = initial_bee + bee_diff * sigmoid;
                

                // Setup for alpha 
                /*
                double alpha_t;
                    if (phy_time < 200.0) {
                        alpha_t = 0.7;
                    } else {
                        alpha_t = 1.2;
                    }
                */

                //setup for angular distribution at B.C.
                G0->G[idx(i, -j - 1)] = g(vz[i], 1.0, 0.6);
                G0->bG[idx(i, -j - 1)] = g(vz[i], 1.0, 0.5);


                // neutrino
                in->ee[idx(i, -j - 1)] = 1.0 * G0->G[idx(i, -j - 1)]; 
                in->xx[idx(i, -j - 1)] = 0.0;
                in->ex_re[idx(i, -j - 1)] = 0.0;
                in->ex_im[idx(i, -j - 1)] = 0.0;

                // anti-neutrino                
                in->bee[idx(i, -j - 1)] = 1.0 * alpha_t * G0->bG[idx(i, -j - 1)] ;
                in->bxx[idx(i, -j - 1)] = 0.0;
                in->bex_re[idx(i, -j - 1)] = 0.0;
                in->bex_im[idx(i, -j - 1)] = 0.0;
                

                // right boundary (open, 3rd order extrapolation)
                // neutrino
                in->ee[idx(i, nz + j)] = 3 * in->ee[idx(i, nz + j - 1)] - 3 * in->ee[idx(i, nz + j - 2)] + in->ee[idx(i, nz + j - 3)];
                in->xx[idx(i, nz + j)] = 3 * in->xx[idx(i, nz + j - 1)] - 3 * in->xx[idx(i, nz + j - 2)] + in->xx[idx(i, nz + j - 3)];
                in->ex_re[idx(i, nz + j)] = 3 * in->ex_re[idx(i, nz + j - 1)] - 3 * in->ex_re[idx(i, nz + j - 2)] + in->ex_re[idx(i, nz + j - 3)];
                in->ex_im[idx(i, nz + j)] = 3 * in->ex_im[idx(i, nz + j - 1)] - 3 * in->ex_im[idx(i, nz + j - 2)] + in->ex_im[idx(i, nz + j - 3)];
                // anti-neutrino
                in->bee[idx(i, nz + j)] = 3 * in->bee[idx(i, nz + j - 1)] - 3 * in->bee[idx(i, nz + j - 2)] + in->bee[idx(i, nz + j - 3)];
                in->bxx[idx(i, nz + j)] = 3 * in->bxx[idx(i, nz + j - 1)] - 3 * in->bxx[idx(i, nz + j - 2)] + in->bxx[idx(i, nz + j - 3)];
                in->bex_re[idx(i, nz + j)] = 3 * in->bex_re[idx(i, nz + j - 1)] - 3 * in->bex_re[idx(i, nz + j - 2)] + in->bex_re[idx(i, nz + j - 3)];
                in->bex_im[idx(i, nz + j)] = 3 * in->bex_im[idx(i, nz + j - 1)] - 3 * in->bex_im[idx(i, nz + j - 2)] + in->bex_im[idx(i, nz + j - 3)];


            }
            
        }
    }
#endif



}



/* v0 = v1 + a * v2 */
void NuOsc::vectorize(FieldVar *v0, const FieldVar *v1, const real a, const FieldVar *v2)
{
#pragma omp parallel for collapse(2)
#pragma acc parallel loop collapse(2) independent // default(present)
    for (int i = 0; i < nvz; i++)
        for (int j = 0; j < nz; j++)
        {
            int k = idx(i, j);
            v0->ee[k] = v1->ee[k] + a * v2->ee[k];
            v0->xx[k] = v1->xx[k] + a * v2->xx[k];
            v0->ex_re[k] = v1->ex_re[k] + a * v2->ex_re[k];
            v0->ex_im[k] = v1->ex_im[k] + a * v2->ex_im[k];
            v0->bee[k] = v1->bee[k] + a * v2->bee[k];
            v0->bxx[k] = v1->bxx[k] + a * v2->bxx[k];
            v0->bex_re[k] = v1->bex_re[k] + a * v2->bex_re[k];
            v0->bex_im[k] = v1->bex_im[k] + a * v2->bex_im[k];
        }
}

// v0 = v1 + a * ( v2 + v3 )
void NuOsc::vectorize(FieldVar *v0, const FieldVar *v1, const real a, const FieldVar *v2, const FieldVar *v3)
{
#pragma omp parallel for collapse(2)
#pragma acc parallel loop collapse(2) independent // default(present)
    for (int i = 0; i < nvz; i++)
        for (int j = 0; j < nz; j++)
        {
            int k = idx(i, j);
            v0->ee[k] = v1->ee[k] + a * (v2->ee[k] + v3->ee[k]);
            v0->xx[k] = v1->xx[k] + a * (v2->xx[k] + v3->xx[k]);
            v0->ex_re[k] = v1->ex_re[k] + a * (v2->ex_re[k] + v3->ex_re[k]);
            v0->ex_im[k] = v1->ex_im[k] + a * (v2->ex_im[k] + v3->ex_im[k]);
            v0->bee[k] = v1->bee[k] + a * (v2->bee[k] + v3->bee[k]);
            v0->bxx[k] = v1->bxx[k] + a * (v2->bxx[k] + v3->bxx[k]);
            v0->bex_re[k] = v1->bex_re[k] + a * (v2->bex_re[k] + v3->bex_re[k]);
            v0->bex_im[k] = v1->bex_im[k] + a * (v2->bex_im[k] + v3->bex_im[k]);
        }
}

void NuOsc::step_rk4()
{
    // Step-1
    updateBufferZone(v_stat);
    calRHS(v_rhs, v_stat);
    vectorize(v_pre, v_stat, 0.5 * dt, v_rhs);

    // Step-2
    updateBufferZone(v_pre);
    calRHS(v_cor, v_pre);
    vectorize(v_rhs, v_rhs, 2.0, v_cor);
    vectorize(v_cor, v_stat, 0.5 * dt, v_cor);
    swap(&v_pre, &v_cor);

    // Step-3
    updateBufferZone(v_pre);
    calRHS(v_cor, v_pre);
    vectorize(v_rhs, v_rhs, 2.0, v_cor);
    vectorize(v_cor, v_stat, dt, v_cor);
    swap(&v_pre, &v_cor);

    // Step-4
    updateBufferZone(v_pre);
    calRHS(v_cor, v_pre);
    vectorize(v_pre, v_stat, 1.0 / 6.0 * dt, v_cor, v_rhs);
    swap(&v_pre, &v_stat);

    phy_time += dt;
}

/*---------------------------------------------------------------------------*/

#endif // __NUOSC__

/*---------------------------------------------------------------------------*/
