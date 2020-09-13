import numpy as np
import query
import escaner_tools as et
import matplotlib.pyplot as plt


filenames = ["19_2_21_B3V2", "19_2_21_B3P3", "20_5_26_B1H2"]


for i in filenames:
    xyz = query.get_sparse_data("database/"+i+".txt")

    et.data_plot(xyz, "", "charts/compare/"+i+"3D.png")

    et.image_plot(xyz, "", "charts/compare/"+i+"GS.png")

    xs = et.get_cross_section(xyz)
    norm_data = et.normalize(xs)
    plt.figure()
    plt.plot(norm_data)
    plt.savefig("charts/compare/"+i+"XS.png")
    plt.close()
