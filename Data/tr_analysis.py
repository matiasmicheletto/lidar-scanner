import seaborn as sns
import matplotlib.pyplot as plt
import query
import pandas as pd

""" 

Graficar influencia de los tratamientos en la temperatura del suelo y viento

"""

# Cargar datos de la base de datos
data = query.get_database()

# Restructurar para convertir en dataframe
temp = []
wnd = []
for item in data:
    if "wnd_max_0" in item: # Si tiene este atributo, tiene los demas
        temp.append({"Prof.": 0, "Tratamiento": item["treatment"], "Temp.": item["temp_l_0"]})
        temp.append({"Prof.": 5, "Tratamiento": item["treatment"], "Temp.": item["temp_l_5"]})
        temp.append({"Prof.": 10,"Tratamiento": item["treatment"], "Temp.": item["temp_l_10"]})
        temp.append({"Prof.": 0, "Tratamiento": item["treatment"], "Temp.": item["temp_s_0"]})
        temp.append({"Prof.": 5, "Tratamiento": item["treatment"], "Temp.": item["temp_s_5"]})
        temp.append({"Prof.": 10,"Tratamiento": item["treatment"], "Temp.": item["temp_s_10"]})

        wnd.append({"Altura": 0,  "Tratamiento": item["treatment"], "Vel": item["wnd_max_0"]})
        wnd.append({"Altura": 30, "Tratamiento": item["treatment"], "Vel": item["wnd_max_30"]})
        wnd.append({"Altura": 200,"Tratamiento": item["treatment"], "Vel": item["wnd_max_200"]})
        wnd.append({"Altura": 0,  "Tratamiento": item["treatment"], "Vel": item["wnd_min_0"]})
        wnd.append({"Altura": 30, "Tratamiento": item["treatment"], "Vel": item["wnd_min_30"]})
        wnd.append({"Altura": 200,"Tratamiento": item["treatment"], "Vel": item["wnd_min_200"]})
        wnd.append({"Altura": 0,  "Tratamiento": item["treatment"], "Vel": item["wnd_prom_0"]})
        wnd.append({"Altura": 30, "Tratamiento": item["treatment"], "Vel": item["wnd_prom_30"]})
        wnd.append({"Altura": 200,"Tratamiento": item["treatment"], "Vel": item["wnd_prom_200"]})

# Convertir a dataframe de pandas
d_temp = pd.DataFrame.from_dict(temp)
d_wnd = pd.DataFrame.from_dict(wnd)

# Grafico de cajas
plt.subplot(121)
sns.boxplot(x="Prof.", y="Temp.", hue="Tratamiento", data=d_temp, palette="Set1")
plt.xlabel("Prof. (cm)")
plt.ylabel("Temp. (°C)")
plt.title("Temperatura del suelo")

plt.subplot(122)
sns.boxplot(x="Altura", y="Vel", hue="Tratamiento", data=d_wnd, palette="Set1")
plt.xlabel("Altura (cm)")
plt.ylabel("Velocidad (km/h)")
plt.title("Velocidad del viento")

plt.show()
