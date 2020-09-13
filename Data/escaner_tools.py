from mpl_toolkits import mplot3d 
import matplotlib.pyplot as plt 
from statistics import stdev
from scipy.interpolate import griddata
import numpy as np
import constants


def normalize(a): # Excalar al rango -.5 a .5
    mx = max(a)
    mn = min(a)
    return [(d-mn)/(mx-mn)-0.5 for d in a] 
 

def moving_average(a, n=3): # Media movil
    ret = np.cumsum(a, dtype=float)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n


def zero_cross_cnt(a): # Cruces por cero del arreglo
    return sum(abs(np.diff(np.sign(a))))/2


def data_plot(xyz, title = None, save_name = None):
    """ Grafica la superficie definida por los datos dispersos de la tupla xyz """

    if not xyz: 
        return 
    
    x = [n for n in xyz[0]]
    y = [n for n in xyz[1]]
    z = [n for n in xyz[2]]

    # Graficar

    #fig = plt.figure(figsize = (9, 6)) 
    fig = plt.figure() 
    ax = plt.axes(projection ="3d") 
    ax.set_zlim3d(0,200) # Para todas las graficas, misma escala vertical

    # Surf
    trisrf = ax.plot_trisurf(x, y, z, linewidth = 0.2, antialiased = True, cmap = plt.get_cmap('afmhot'))

    # Scatter
    sctt = ax.scatter(x, y, z, alpha = 0.5, s = 0.5, color = [0,0,0]) 

    fig.colorbar(trisrf, ax = ax, shrink = 0.5, aspect = 5)   
    if title is not None:
        plt.title(title) 
    ax.grid(b = True, color ='grey', linestyle ='-.', linewidth = 0.3, alpha = 0.2)  
    ax.set_xlabel('Eje X', fontweight ='bold')  
    ax.set_ylabel('Eje Y', fontweight ='bold')  
    ax.set_zlabel('Altura (mm)', fontweight ='bold') 
    
    if save_name is not None:
        plt.savefig(save_name)
        print("Archivo guardado como "+save_name)
        plt.close()
    else:
        plt.show(block=False) 


def sparse_to_grid(xyz, transpose = False):
    """ Obtener grilla regular de 256x256 con los datos normalizados de 0 a 255"""

    grid_x, grid_y = np.mgrid[0:max(xyz[0]):255j, 0:max(xyz[1]):255j]

    # Ajustar dimensiones de matrices
    points = []
    for i,p in enumerate(xyz[0]):
        points.append( [round(xyz[0][i]), round(xyz[1][i])] )
    values = [round(float(i)/max(xyz[2])*255) for i in xyz[2]]

    # Interpolar datos
    grid_z = griddata(points, values, (grid_x, grid_y), method='cubic')

    res = np.nan_to_num(grid_z)

    return res if not transpose else res.T


def image_plot(xyz, title = None, save_name = None):
    """ Graficar modelo como imagen en escala de grises """

    # Convertir a grilla regular
    grid = sparse_to_grid(xyz)

    # Graficar
    plt.figure()
    plt.imshow(grid, origin='lower')
    if save_name is not None:
        plt.savefig(save_name)
        print("Archivo guardado como "+save_name)
        plt.close()
    else:    
        plt.show(block=False)


def get_image_entropy(xyz):
    """ Calcula la entropia de Shannon"""

    grid = sparse_to_grid(xyz)
    marg = np.histogramdd(np.ravel(grid), bins = 256)[0]/grid.size
    marg = list(filter(lambda p: p > 0, np.ravel(marg)))
    entropy = -np.sum(np.multiply(marg, np.log2(marg)))
    return entropy


def get_cross_section(xyz, orientation = "H"):
    """ Obtener la curva de seccion transversal horizontal ("H") o vertical ("V") """

    # Convertir a grilla regular
    grid = sparse_to_grid(xyz, orientation == "H")

    # Sumar columnas
    data = np.nansum(grid, axis=0)

    # Retornar sumas quitando las que den 0
    return data[data!=0]


def get_section_noise(xyz, orientation = "H"):
    xs = get_cross_section(xyz, orientation)
    nd = normalize(xs)
    dnd = np.diff(nd)
    mdnd = moving_average(dnd,30)
    noise = np.subtract(dnd, np.append(mdnd, np.zeros(len(dnd)-len(mdnd))))
    return zero_cross_cnt(noise)


def get_max_height(xyz):
    """ Obtener la altura maxima del modelo """
    return max(xyz[2])


def get_std_height(xyz):
    """ Obtener desvio estandar de los datos """
    return stdev(xyz[2])


def get_volume(xyz):
    """ Obtener el volumen bajo la envolvente """
    z = xyz[2]
    return sum(z) / len(z) * constants.area / 1000000

