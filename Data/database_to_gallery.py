import query
from escaner_tools import data_plot

"""

Genera el grafico 3D de cada modelo medido y lo guarda en la carpeta "gallery"

"""

# Todos los ensayos sin errores
filenames = query.get_database()

for filename in filenames:
    xyz = query.get_sparse_data("database/"+filename)
    data_plot(xyz, "Datos "+filename, "gallery/"+filename[:-4]+".png")