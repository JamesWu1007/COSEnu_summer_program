### To generate the MP4!!!

import os
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm

# path for data & output
data_dir = '/hetghome/james/COSEnu/output_for_project/ffs_collective_alpha_0.1/10000_2_0.5/' ## path of data
file_template = 'rho_{}.dat'

frames_dir = '/hetghome/james/COSEnu/png' ##output fig path & name
output_file = '/hetghome/james/COSEnu/for_video_page/alpha0.1_vz2.mp4' ##output MP4 path & name

# create dir. for output fig
if not os.path.exists(frames_dir):
    os.makedirs(frames_dir)

# read all files
file_indices = range(0, 24000, 80)
file_names = [os.path.join(data_dir, file_template.format(i)) for i in file_indices]

# define read_data
def read_data(file_name):
    return np.loadtxt(file_name)

# initialize fig.
fig, axs = plt.subplots(4, 1, figsize=(10, 3))  # Increased figure height for better spacing

# setup for color
colors = ['#EE7700', '#0044BB', '#00FFFF','#FF00FF' ]

#set up for each picture!!!
for i, file_name in tqdm(enumerate(file_names), total=len(file_names), desc="Generating frames"):
    data = read_data(file_name)
    
    mask1 = data[:, 1] == 1
    filtered_data1 = data[mask1]
    #mask2 = data[:, 1] == 1
    mask2 = data[:, 1] == -1
    filtered_data2 = data[mask2]

    z = filtered_data1[:, 0]
    rho_ee = filtered_data1[:, 2]
    rho_xx = filtered_data1[:, 3]
    rho_bee = filtered_data2[:, 6] 
    rho_bxx = filtered_data2[:, 7]
    
    axs[0].clear()
    axs[1].clear()
    axs[2].clear()
    axs[3].clear()
    
    axs[0].plot(z, rho_ee, label=r'$\nu_e$', color=colors[0])
    axs[0].plot(z, rho_bee, label=r'$\bar{\nu_e}$', color=colors[1])
    axs[0].set_ylabel(r'$\rho_{ee}$')
    axs[0].set_xlabel(r'$z$')
    axs[0].set_xticks(np.arange(0, 1001, 100))
    axs[0].set_ylim(-0.05, 1.05)
    axs[0].legend(loc='upper right')

    axs[1].plot(z, rho_xx, label=r'$\nu_x$', color=colors[2])
    axs[1].set_ylabel(r'$\rho_{xx}$')
    axs[1].set_xticks(np.arange(0, 1001, 100))
    axs[1].set_ylim(-0.05, 1.05)
    axs[1].legend(loc='upper right')
    
    axs[2].plot(z, rho_bxx, label=r'$\bar{\nu_x}$', color=colors[3])
    axs[2].set_ylabel(r'$\rho_{xx}$')
    axs[2].set_xticks(np.arange(0, 1001, 100))
    axs[2].set_ylim(-0.05, 1.05)
    axs[2].legend(loc='upper right')

    
    axs[3].plot(z, rho_bee, label=r'$\bar{\nu_e}$', color=colors[1])
    axs[3].set_ylabel(r'$\rho_{ee}$')
    axs[3].set_xlabel(r'$z$')
    axs[3].set_xticks(np.arange(0, 1001, 100))
    axs[3].set_ylim(-0.05, 2.05)
    axs[3].legend(loc='upper right')
   


    # Adjusted title position
    fig.suptitle(f'spatial distribution of density, t = {i * 80 * 0.05:.2f} ($\mu^{-1}$)', y=0.98) 
    plt.tight_layout(rect=[0, 0, 1, 0.95])  # Adjusted layout to avoid overlap with title
    
    output_path = os.path.join(frames_dir, f'frame_{i:04d}.png')
    plt.savefig(output_path)

plt.close(fig)


# use "ffmpeg" to generate MP4
os.system(f'ffmpeg -r 10 -i {frames_dir}/frame_%04d.png -vcodec libx264 -pix_fmt yuv420p {output_file}')

print(f"SUCCESS!!!")
print(f"MP4 file saved as: {output_file}")
