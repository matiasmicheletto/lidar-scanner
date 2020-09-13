import query 
from escaner_tools import get_cross_section
import matplotlib.pyplot as plt
import numpy as np

"""

Genera la curva de sección transversal a la línea de siembra y guarda el grafico en la carpeta "./xsection" (debe existir).

"""

items = query.get_database()

for item in items:
    filename = item["filename"]
    figname = "xsection/"+filename[:-4]+".png"

    xyz = query.get_sparse_data("database/"+filename)
    data = get_cross_section(xyz, item["orientation"])
    
    # Normalizar
    mx = max(data)
    mn = min(data)
    norm_data = [(d-mn)/(mx-mn)-0.5 for d in data]

    plt.plot(norm_data)
    plt.savefig(figname)
    plt.close()
    
    print("Guardado "+figname)