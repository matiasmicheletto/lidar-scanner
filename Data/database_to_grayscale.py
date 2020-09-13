import query 
from escaner_tools import image_plot

"""

Convertir modelos en imagenes en escala de grises.

"""

filenames = query.get_filenames_with_condition(lambda item: item["status"] == "ok")

for filename in filenames:
    xyz = query.get_sparse_data("database/"+filename)
    image_plot(xyz, "Datos "+filename, "grayscale/im_"+filename[:-4]+".png")