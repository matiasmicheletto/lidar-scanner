import matplotlib.pyplot as plt
from query import get_items_with_condition
import constants


"""

Obtener grafico de datos dispersos de cualesquiera dos variables del dataset.
En el grafico se diferencian por tratamiento

"""

# Variables a graficar
var_y = "volume"
var_x = "median"
# Mostrar identificador de cada dato
tags = False

#  Colores de los tratamientos
cols = { 
    "H":"tab:red",
    "P":"tab:blue",
    "V":"tab:green"
}

var_y_name = constants.get_attribute_name(var_y)
var_x_name = constants.get_attribute_name(var_x)

items = get_items_with_condition(lambda item: var_y in item and var_x in item)
#items = get_items_with_condition(lambda item: item["treatment"] == "V")

x = {"H":[],"P":[],"V":[]} # Variable independiente
y = {"H":[],"P":[],"V":[]} # Variable dependiente
l = {"H":[],"P":[],"V":[]} # Etiquetas

c = [] # Color del dato

# Generar arreglos de datos agrupados por tratamiento
for item in items:
    x[item["treatment"]].append(item[var_x])
    y[item["treatment"]].append(item[var_y])
    if tags:
        l[item["treatment"]].append(item["filename"][:1]+"_"+item["block"]+item["treatment"]+item["rep"])

# Graficar
fig, ax = plt.subplots()
for tr in ["H", "P", "V"]:
    ax.scatter(x[tr],y[tr], c=cols[tr], label=tr)
    # Anotar etiquetas
    if tags:
        for i,txt in enumerate(l[tr]): 
            ax.annotate(l[tr][i], (x[tr][i]+0.1, y[tr][i]+0.2)) 

ax.grid(True)
ax.legend()
plt.title("{} vs {}".format(var_x_name, var_y_name))
plt.ylabel(var_y_name)
plt.xlabel(var_x_name)
plt.show()
