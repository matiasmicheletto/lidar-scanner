import escaner_tools as et 
import query as q 
import matplotlib.pyplot as plt 
from random import sample
import pandas as pd 
import seaborn as sns

filename = 'high_res.csv'


def rand_subsample(data, p):
    """ Submuestreo p% aleatorio sin reemplazo """
    z = data[2]
    n = round(len(z)*p/100)
    return sample(z, n)


def get_height(data):
    """ Altura media, min y max"""
    return (min(data), max(data), sum(data)/len(data))


if __name__ == "__main__":
    xyz = q.get_sparse_data("database/"+filename)
    #et.data_plot(xyz, "Datos "+filename)  

    heights = []
    for p in [1, 2, 3, 4, 5, 10, 25, 50, 75, 100]:
        for r in range(100):
            ssampled = rand_subsample(xyz, p)
            (mn, mx, avg) = get_height(ssampled)
            heights.append([str(p)+"%", mn, "Min"])
            heights.append([str(p)+"%", mx, "Max"])
            heights.append([str(p)+"%", avg, "Avg"])
    
    df = pd.DataFrame(heights, columns=["Freq","Height", "Attr"])    
    print(df.shape)
    
    plt.subplot(121)
    plt.grid()
    sns.boxplot(x="Freq", y="Height", data=df.loc[df["Attr"] == "Min"], palette="Set1")    
    plt.xlabel("Proporción de submuestreo")
    plt.ylabel("Altura")
    plt.title("Altura mínima")

    plt.subplot(122)
    plt.grid()
    sns.boxplot(x="Freq", y="Height", data=df.loc[df["Attr"] == "Max"], palette="Set1")    
    plt.xlabel("Proporción de submuestreo")
    plt.ylabel("Altura")
    plt.title("Altura máxima")

    
    # plt.subplot(313)
    # plt.grid()
    # sns.boxplot(x="Freq", y="Height", data=df.loc[df["Attr"] == "Avg"], palette="Set1")    
    # plt.xlabel("Proporción de submuestreo")
    # plt.ylabel("Altura")
    # plt.title("Altura promedio")

    plt.show()
