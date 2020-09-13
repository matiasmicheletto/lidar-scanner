import query
import json
import escaner_tools as et
from statistics import median


"""

A partir de los datos csv del modelo, actualiza variables en el resumen database.json

"""


# Obtener la base de datos actual
items = query.get_database()

# Para cada item, obtener variables volumetricas
for item in items:
    xyz = query.get_sparse_data("database/"+item["filename"])
    
    # Obtener altura maxima
    zm = et.get_max_height(xyz)

    # Obtener desvio de los datos
    std = et.get_std_height(xyz)

    # Calcular mediana
    md = median(xyz[2])

    # Calcular volumen (prom. z * area)
    v = et.get_volume(xyz)

    # Obtener entropia de la imagen
    entpy = et.get_image_entropy(xyz)

    # Calcular nivel de ruido de la seccion transversal
    n = et.get_section_noise(xyz, item["orientation"])

    # Cargar nuevas variables al item
    item["max_height"] = round(zm,2)
    item["std_dev"] = round(std, 2)
    item["median"] = round(md, 2)
    item["volume"] = round(v,2)
    item["entropy"] = round(entpy,2)
    item["sec_noise"] = n

    item["density_h"] = None
    item["density_v"] = None

    # Varibales que dependen de la materia seca
    item["density"] = round( (item["ms_v"]+item["ms_h"])/v,2 )
    

    print("Actualizado "+item["filename"])

# En lugar de reescribir base de datos, se hace una copia para comparar
with open('new_database.json', 'w') as outfile:
    json.dump(items, outfile)
