#if not defined(__INITIALIZE__)
#define __INITIALIZE__

/*
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
    double signu  = 0.6;
    double sigbnu = 0.5;
    double alpha  = 0.9;

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
           
            // 設置spatial distribution
            double gaussian = gauss(Z[j], 0, 50);
            
            /*
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


            G0->G[idx(i, j)] = g(vz[i], 1.0, signu);
            G0->bG[idx(i, j)] = alpha * g(vz[i], 1.0, sigbnu);

            v_stat->ee[idx(i, j)]    = G0->G[idx(i, j)] ;
            
            v_stat->xx[idx(i, j)]    = 0.5 * G0->G[idx(i, j)] * (1.0 - eps_(Z[j], 0.0, perturbation_size));
            v_stat->ex_re[idx(i, j)] = 0.5 * G0->G[idx(i, j)] * (0.0 + eps(Z[j], 0.0, perturbation_size));
            v_stat->ex_im[idx(i, j)] = -0.0;

            v_stat->bee[idx(i, j)]    = 0.5 * G0->bG[idx(i, j)] * (1.0 + eps_(Z[j], 0.0, perturbation_size)); 
            v_stat->bxx[idx(i, j)]    = 0.5 * G0->bG[idx(i, j)] * (1.0 - eps_(Z[j], 0.0, perturbation_size));
            v_stat->bex_re[idx(i, j)] = 0.5 * G0->bG[idx(i, j)] * (0.0 + eps(Z[j], 0.0, perturbation_size));
            v_stat->bex_im[idx(i, j)] = 0.0;
        
            

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


