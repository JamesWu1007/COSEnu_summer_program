#if not defined(__INITIALIZE__)
#define __INITIALIZE__

/*
//for COSEnu advection part
定義一個box car profile
double box_car_profile(double z) {
    return (z >= -100.0 && z <= 100.0) ? 1.0 : 0.0;
}
*/

/*---------------------------------------------------------------------------*/

void NuOsc::initialize()
{
/*
    Initialize the components of \rho and \bar{\rho}
    here.
*/

    
    // for COSEnu collective part
    /*
    double signu  = 0.6;
    double sigbnu = 0.5;
    double alpha  = 0.9;
    */
    

    std::ofstream g_file("G0.bin",std::ofstream::out | std::ofstream::binary);
    if(!g_file)
    {
        std::cout << "Unable to open " << "G0.bin" << " file from NuOsc::initialise." 
        << "Will not be storing initial angular profiles.\n";
    }


    for (int i = 0; i < nvz; i++)
    {
        for (int j = 0; j < nz; j++)
        {

            /*
            //for COSEnu advection & vaccum part

            // set spatial distribution
            double gaussian = gauss(Z[j], 0, 50);

            v_stat->ee[idx(i, j)]    = gaussian;
            //v_stat->ee[idx(i, j)]    = box_car_profile(Z[j]);
            v_stat->xx[idx(i, j)]    = 0.0 ;
            v_stat->ex_re[idx(i, j)] = 0.0 ;
            v_stat->ex_im[idx(i, j)] = 0.0 ;

            v_stat->bee[idx(i, j)]    = 0.0;
            v_stat->bxx[idx(i, j)]    = 0.0;
            v_stat->bex_re[idx(i, j)] = 0.0;
            v_stat->bex_im[idx(i, j)] = 0.0;
            */
           

            /*
            // for COSEnu collective part
            G0->G[idx(i, j)] = g(vz[i], 1.0, signu);
            G0->bG[idx(i, j)] = alpha * g(vz[i], 1.0, sigbnu);

            v_stat->ee[idx(i, j)]    = 0.5 * G0->G[idx(i, j)] * (1.0 + eps_(Z[j], 0.0, perturbation_size)); 
            v_stat->xx[idx(i, j)]    = 0.5 * G0->G[idx(i, j)] * (1.0 - eps_(Z[j], 0.0, perturbation_size));
            v_stat->ex_re[idx(i, j)] = 0.5 * G0->G[idx(i, j)] * (0.0 + eps(Z[j], 0.0, perturbation_size));
            v_stat->ex_im[idx(i, j)] = -0.0;

            v_stat->bee[idx(i, j)]    = 0.5 * G0->bG[idx(i, j)] * (1.0 + eps_(Z[j], 0.0, perturbation_size)); 
            v_stat->bxx[idx(i, j)]    = 0.5 * G0->bG[idx(i, j)] * (1.0 - eps_(Z[j], 0.0, perturbation_size));
            v_stat->bex_re[idx(i, j)] = 0.5 * G0->bG[idx(i, j)] * (0.0 + eps(Z[j], 0.0, perturbation_size));
            v_stat->bex_im[idx(i, j)] = 0.0;
            */
            


            
            // for FFS!!!
            // left boundary (z = 0)
            if (j == 0 && vz[i] > 0)
            {
                //neutrino
                v_stat->ee[idx(i, j)] = 0.5 * (1.0 + sqrt(1 - perturbation_size * perturbation_size));
                v_stat->xx[idx(i, j)] = 0.5 * (1.0 - sqrt(1 - perturbation_size * perturbation_size));
                v_stat->ex_re[idx(i, j)] = 0.5 * perturbation_size;
                v_stat->ex_im[idx(i, j)] = 0.0;
                //anti-neutrino
                v_stat->bee[idx(i, j)] = 0.0;
                v_stat->bxx[idx(i, j)] = 0.0;
                v_stat->bex_re[idx(i, j)] = 0.0;
                v_stat->bex_im[idx(i, j)] = 0.0;
            }
            // right boundary (z = L_z)
            else if (j == nz - 1 && vz[i] < 0)
            {
                //neutrino
                v_stat->ee[idx(i, j)] = 0.0;
                v_stat->xx[idx(i, j)] = 0.0;
                v_stat->ex_re[idx(i, j)] = 0.0;
                v_stat->ex_im[idx(i, j)] = 0.0;
                //anti-neutrino
                v_stat->bee[idx(i, j)] = 0.5 * (1.0 + sqrt(1 - perturbation_size * perturbation_size)) * 2.0;
                v_stat->bxx[idx(i, j)] = 0.5 * (1.0 - sqrt(1 - perturbation_size * perturbation_size)) * 2.0 ;
                v_stat->bex_re[idx(i, j)] = 0.5 * perturbation_size;
                v_stat->bex_im[idx(i, j)] = 0.0;
            }
            
            
            /*
            // for FFS time-depend case!!!
            // left boundary (z = 0)

            G0->G[idx(i, j)] = g(vz[i], 1.0, 0.6);
            G0->bG[idx(i, j)] = 0.85 * g(vz[i], 1.0, 0.5);

            if (j == 0 && vz[i] > 0)
            {
                //neutrino
                v_stat->ee[idx(i, j)] = 0.5 * G0->G[idx(i, j)] * (1.0 + sqrt(1 - perturbation_size * perturbation_size));
                v_stat->xx[idx(i, j)] = 0.5 * G0->G[idx(i, j)] * (1.0 - sqrt(1 - perturbation_size * perturbation_size));
                v_stat->ex_re[idx(i, j)] = 0.5 * perturbation_size;
                v_stat->ex_im[idx(i, j)] = 0.0;
                //anti-neutrino
                v_stat->bee[idx(i, j)] = 0.5 * G0->bG[idx(i, j)] * (1.0 + sqrt(1 - perturbation_size * perturbation_size));
                v_stat->bxx[idx(i, j)] = 0.5 * G0->bG[idx(i, j)] * (1.0 - sqrt(1 - perturbation_size * perturbation_size));
                v_stat->bex_re[idx(i, j)] = 0.5 * perturbation_size; ;
                v_stat->bex_im[idx(i, j)] = 0.0;
            }
            */
            
            
            

            g_file.write((char *)&G0->G [idx(i, j)], sizeof(double)); 
            g_file.write((char *)&G0->bG[idx(i, j)], sizeof(double));
        }
    }
    updateBufferZone(v_stat);
    std::cout << "Simulation state initialized." << std::endl;
    g_file.close();
}

/*---------------------------------------------------------------------------*/

#endif // __INITIALIZE__

/*---------------------------------------------------------------------------*/

