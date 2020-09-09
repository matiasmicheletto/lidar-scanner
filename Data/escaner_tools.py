from mpl_toolkits import mplot3d 
import matplotlib.pyplot as plot 
from statistics import stdev
from scipy.interpolate import griddata
import numpy as np
import constants


def data_plot(xyz, title = None, save_name = None):
    """ Grafica la superficie definida por los datos dispersos de la tupla xyz """

    if not xyz: 
        return 
    
    x = [n for n in xyz[0]]
    y = [n for n in xyz[1]]
    z = [n for n in xyz[2]]

    # Graficar

    fig = plot.figure(figsize = (9, 6)) 
    ax = plot.axes(projection ="3d") 
    ax.set_zlim3d(0,350) # Para todas las graficas, misma escala vertical

    # Surf
    trisrf = ax.plot_trisurf(x, y, z, linewidth = 0.2, antialiased = True, cmap = plot.get_cmap('afmhot'))

    # Scatter
    sctt = ax.scatter(x, y, z, alpha = 0.5, s = 0.5, color = [0,0,0]) 

    fig.colorbar(trisrf, ax = ax, shrink = 0.5, aspect = 5)   
    if title is not None:
        plot.title(title) 
    ax.grid(b = True, color ='grey', linestyle ='-.', linewidth = 0.3, alpha = 0.2)  
    ax.set_xlabel('Eje X', fontweight ='bold')  
    ax.set_ylabel('Eje Y', fontweight ='bold')  
    ax.set_zlabel('Altura (mm)', fontweight ='bold') 
    
    if save_name is not None:
        plot.savefig(save_name)
        print("Archivo guardado como "+save_name)
        plot.close()
    else:
        plot.show() 


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

    return grid_z if not transpose else grid_z.T


def image_plot(xyz, title = None, save_name = None):
    """ Graficar modelo como imagen en escala de grises """

    # Convertir a grilla regular
    grid = sparse_to_grid(xyz)

    # Graficar
    plot.imshow(grid, origin='lower')
    if save_name is not None:
        plot.savefig(save_name)
        print("Archivo guardado como "+save_name)
        plot.close()
    else:    
        plot.show()


def get_cross_section(xyz, orientation = "H"):
    """ Obtener la curva de seccion transversal horizontal ("H") o vertical ("V") """

    # Convertir a grilla regular
    grid = sparse_to_grid(xyz, orientation == "H")

    # Sumar columnas
    data = np.nansum(grid, axis=0)

    # Retornar sumas quitando las que den 0
    return data[data!=0]


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

