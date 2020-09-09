import os.path
import csv
import json
import constants


def get_database():
    with open('database.json') as json_file:
        data = json.load(json_file)
    return data


def get_items_with_condition(condition):
    """ 
    Retorna una lista de items que cumplen con la consulta.
    Ejemplo:

    >>> get_items_with_condition(lambda item: item['treatment']=="H")
    """
    
    with open('database.json') as json_file:
        data = json.load(json_file)
    return [item for item in data if condition(item)] 


def get_filenames_with_condition(condition):
    """ 
    Retorna una lista de nombres de registros que cumplen con la consulta.
    Ejemplo:

    >>> get_filenames_with_condition(lambda item: item['treatment']=="H")
    """
    
    with open('database.json') as json_file:
        data = json.load(json_file)
        filenames = [item["filename"] for item in data if condition(item)] 
    return filenames


def get_sparse_data(filename):
    """ Lee el archivo csv y devuelve la tupla con vectores (x,y,z) en mm"""
    
    if not os.path.isfile(filename):
        print("El archivo {} no existe".format(filename))
        return None
    
    with open(filename) as csvfile:
        reader = csv.reader(csvfile, delimiter = ',')
        
        # Obtener vectores
        rows = list(reader)

    # Expresar coordenadas x,y en mm    
    x = [int(r[0])/constants.steps_mm for r in rows]
    y = [int(r[1])/constants.steps_mm for r in rows]
    z = [int(r[2]) for r in rows]

    # Invertir z
    zAlt = max(z) # Maxima altura registrada
    #print("Altura escaner: {}".format(zAlt))
    z = [(zAlt - n) if n < constants.zMax else 0 for n in z]

    return (x,y,z)
