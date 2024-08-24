import os
import numpy as np
import matplotlib.pyplot as plt
from tqdm import tqdm

# path for data & output
data_dir = '/hetghome/james/COSEnu/output_for_project/ffs_multi_beam_nvz_32/10000_32_0.5/'
file_template = 'rho_{}.dat'
frames_dir = '/hetghome/james/COSEnu/png'
output_file = '/hetghome/james/COSEnu/for_video_page/alpha_1.0_vz32.mp4'
#output_file = '/hetghome/james/COSEnu/for_video_page/vaccum_col_alpha1.0_vz16.mp4'

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
fig, ax = plt.subplots(figsize=(8, 2))

# setup for color
colors = ['#EE7700', '#0044BB', '#00FFFF', '#FF00FF']

# set up for each picture!!!
for i, file_name in tqdm(enumerate(file_names), total=len(file_names), desc="Generating frames"):
    data = read_data(file_name)
    mask1 = data[:, 1] == 1
    mask2 = data[:, 1] == -1
    filtered_data1 = data[mask1]
    filtered_data2 = data[mask2]

    z = filtered_data1[:, 0]
    rho_ee = filtered_data1[:, 2]
    rho_bee = filtered_data2[:, 6] 

    ax.clear()
    ax.plot(z, rho_ee, label=r'$\nu_e$', color=colors[0])
    ax.plot(z, rho_bee, label=r'$\bar{\nu_e}$', color=colors[1])
    ax.set_ylabel(r'$\rho_{ee}$')
    ax.set_xlabel(r'$z$')
    ax.set_xticks(np.arange(0, 1001, 100))
    ax.set_ylim(-0.05, 1.05)
    ax.legend(loc='upper right')

    # Adjusted title position
    plt.title(f'Spatial distribution of density, t = {i * 80 * 0.05:.2f} ($\mu^{{-1}}$)', y=1.02)

    plt.tight_layout()
    output_path = os.path.join(frames_dir, f'frame_{i:04d}.png')
    plt.savefig(output_path)

plt.close(fig)

# use "ffmpeg" to generate MP4
os.system(f'ffmpeg -r 10 -i {frames_dir}/frame_%04d.png -vcodec libx264 -pix_fmt yuv420p {output_file}')

print(f"SUCCESS!!!")
print(f"MP4 file saved as: {output_file}")