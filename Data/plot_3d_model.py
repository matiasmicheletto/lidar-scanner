from escaner_tools import data_plot
from query import get_sparse_data

filename = "20_5_26_B3V3.txt" # Archivo a graficar

xyz = get_sparse_data("database/"+filename)    
data_plot(xyz, "Datos "+filename)