import matplotlib.pyplot as plt
import numpy as np
from sklearn import datasets, linear_model
from sklearn.metrics import mean_squared_error, r2_score
from query import get_items_with_condition
from constants import area

items = get_items_with_condition(lambda item: item["treatment"] == "V")

X = []
y = []
for item in items:
    X.append([item["volume"]])
    y.append(item["ms_v"] / area * 1e7)

X = np.array(X)
y = np.array(y)

regr = linear_model.LinearRegression()
regr.fit(X.reshape(-1,1), y)

y_pred = regr.predict(X)

print('Coeficientes: m =', regr.coef_[0], ' b =',regr.intercept_)
print('Error medio cuadrático: %.2f' % mean_squared_error(y, y_pred))
print('Coeficiente de determinación: %.2f' % r2_score(y, y_pred))

plt.scatter(X, y,  color = 'green', label = 'Datos')
plt.plot(X, y_pred, color = 'red', label = 'Ajuste')
plt.grid()
plt.legend()
plt.title("Volumen vs materia seca vertical")
plt.ylabel("Materia seca vertical (Kg/ha)")
plt.xlabel("Volumen envolvente (L)")
plt.show()