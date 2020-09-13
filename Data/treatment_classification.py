from query import get_database
from sklearn.metrics import confusion_matrix
from plot_confusion_matrix import plot_confusion_matrix

"""

Testear reglas de clasificacion de tratamientos a partir de los modelos escaneados (sin incluir peso seco, temperatura ni intensidad de viento)

"""


items = get_database()

verd = []
pred = []

target_names = ["H", "P", "V"]

for item in items:
    p = ""
    if item["volume"] >= 4:
        p = "V"
    else:
        if item["sec_noise"] >= 16:
            p = "H"
        else:
            p = "P"
    
    pred.append(p)
    verd.append(item["treatment"])
    
cm = confusion_matrix(verd, pred)

print(cm)

plot_confusion_matrix(cm, target_names = target_names)


