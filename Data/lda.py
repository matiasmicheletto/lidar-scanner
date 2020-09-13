
import matplotlib.pyplot as plt
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
import numpy as np

import query


items = query.get_database()


#attributes = ["max_height","median","std_dev","volume","entropy","sec_noise"]
attributes = ["max_height","median","std_dev","volume","entropy","sec_noise","ms_v","ms_h","density"]


treatments = ["H", "P", "V"]
tr = {
    "H": 0,
    "P": 1,
    "V": 2
}

X = []
y = []
for item in items:
    row = []
    for att in attributes:
        row.append(item[att])
    X.append(row)
    y.append(tr[item["treatment"]])

X = np.array(X)
y = np.array(y)

lda = LinearDiscriminantAnalysis(n_components=2)
X_r2 = lda.fit(X, y).transform(X)

colors = ['navy', 'turquoise', 'darkorange']
plt.figure()
for color, i, target_name in zip(colors, [0, 1, 2], treatments):
    plt.scatter(X_r2[y == i, 0], X_r2[y == i, 1], alpha=.8, color=color,
                label=target_name)
plt.legend(loc='best', scatterpoints=1)
plt.title('Análisis Discriminante')
plt.xlabel('LDA1')
plt.ylabel('LDA2')

plt.show()
